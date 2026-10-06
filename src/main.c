#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifndef HOST_TEST
#include <ti/screen.h>
#include <ti/getcsc.h>
#endif

/* Angles are ALWAYS degrees. Zero denotes an unknown input. */
#define PI 3.14159265358979323846
#define EPS 1e-10
#define MAX_LINES 500
static char lines[MAX_LINES][27];
static int nlines;
static const char side[]="abc", angle[]="ABC";
double results[2][7]; /* a,b,c,A,B,C,area */
static double sn(double x) { return sin(x*PI/180.0); }
static double cs(double x) { return cos(x*PI/180.0); }
static double clamp(double x) { return x>1?1:(x< -1?-1:x); }
static void say(const char *fmt,...) {
    char text[220]; int p=0;
    va_list ap; va_start(ap,fmt); vsnprintf(text,sizeof text,fmt,ap); va_end(ap);
    do {
        int k=0;
        if(nlines>=MAX_LINES) return;
        while(text[p] && text[p]!='\n' && k<26) lines[nlines][k++]=text[p++];
        lines[nlines++][k]=0;
        if(text[p]=='\n') ++p;
    } while(text[p]);
}
static void missing_angle(double *A,int k,int i,int j) {
    A[k]=180-A[i]-A[j];
    say("%c=180-%c-%c",angle[k],angle[i],angle[j]);
    say(" =180-%.2f-%2f",A[i],A[j]);
    say("%c=%.2f deg",angle[k],A[k]);
}
static void sine_side(double *s,double *A,int k,int i) {
    s[k]=s[i]*sn(A[k])/sn(A[i]);
    say("%c=%c*sin(%c)/sin(%c)",side[k],side[i],angle[k],angle[i]);
    say(" =%.2f*sin(%.2f)",s[i],A[k]);
    say(" /sin(%.2f)",A[i]);
    say("%c=%.2f",side[k],s[k]);
}
static void cosine_angle(double *s,double *A,int k) {
    int i=(k+1)%3,j=(k+2)%3;
    double r=(s[i]*s[i]+s[j]*s[j]-s[k]*s[k])/(2*s[i]*s[j]);
    A[k]=acos(clamp(r))*180/PI;
    say("cos(%c)=(%c^2+%c^2-%c^2)",angle[k],side[i],side[j],side[k]);
    say(" /(2*%c*%c)",side[i],side[j]);
    say(" =(%.2f^2+%.2f^2",s[i],s[j]);
    say(" -%.2f^2)/",s[k]);
    say(" (2*%.2f*%.2f)",s[i],s[j]);
    say("cos(%c)=%.2f",angle[k],r);
    say("%c=acos(%.2f)",angle[k],clamp(r));
    say("%c=%.2f deg",angle[k],A[k]);
}
static void finish(double *s,double *A,int sol) {
    double p=(s[0]+s[1]+s[2])/2;
    double r=p*(p-s[0])*(p-s[1])*(p-s[2]);
    double area=sqrt(fmax(0,r));
    say(" "); say("HERON'S FORMULA");
    say("p=(a+b+c)/2");
    say(" =(%.2f+%.2f",s[0],s[1]);
    say(" +%.2f)/2=%.2f",s[2],p);
    say("K=sqrt(p*(p-a)*"); say(" (p-b)*(p-c))");
    say("K=sqrt(%.2f*",p);
    say(" %.2f*%.2f*",p-s[0],p-s[1]);
    say(" %.2f)",p-s[2]);
    say("K=sqrt(%.2f)",fmax(0,r));
    say("Area=%.2f",area);
    say(" "); say("FINAL: SOLUTION %d",sol+1);
    for(int i=0;i<3;++i) {
        results[sol][i]=s[i]; results[sol][i+3]=A[i];
        say("%c=%.2f %c=%.2f deg",side[i],s[i],angle[i],A[i]);
    }
    results[sol][6]=area;
    say("Area=%.2f sq units",area);
}
/* Public core: input [a,b,c,A,B,C], return number of solutions. */
int solve(const double *input) {
    double s[3],A[3]; int ns=0,na=0,si=-1,ai=-1;
    nlines=0; memset(results,0,sizeof results);
    say("TRIANGLE WORK / DEGREES");
    for(int i=0;i<3;++i) {
        s[i]=input[i]; A[i]=input[i+3];
        if(!isfinite(s[i]) || !isfinite(A[i]) || s[i]<0 || A[i]<0 || A[i]>=180) {
            say("Invalid input range."); return 0;
        }
        if(s[i]>0) { ++ns; si=i; say("Given %c=%.7g",side[i],s[i]); }
        if(A[i]>0) { ++na; ai=i; say("Given %c=%.7g deg",angle[i],A[i]); }
    }
    if(ns+na!=3) { say("Enter exactly 3 knowns."); return 0; }
    if(ns==0) { say("AAA gives no side scale."); say("Enter at least one side."); return 0; }
    if(ns==3) {
        say("CASE: SSS / COSINES");
        double m=fmax(s[0],fmax(s[1],s[2]));
        if(s[0]/m+s[1]/m+s[2]/m<=2) {
            say("No triangle:"); say("two sides must sum to"); say("more than the third."); return 0;
        }
        /* Compute largest angle first, then another by cosine. */
        int k=0; if(s[1]>s[k]) k=1; if(s[2]>s[k]) k=2;
        int j=(k+1)%3, h=(k+2)%3;
        cosine_angle(s,A,k); cosine_angle(s,A,j); missing_angle(A,h,k,j);
        finish(s,A,0); return 1;
    }
    if(na==2) {
        int k=0; while(A[k]>0) ++k;
        int i=(k+1)%3,j=(k+2)%3;
        say("CASE: %s / SINES",si==k?"ASA":"AAS");
        missing_angle(A,k,i,j);
        if(A[k]<=0) { say("No triangle: angle sum"); say("must be below 180."); return 0; }
        for(int t=0;t<3;++t) if(t!=si) sine_side(s,A,t,si);
        finish(s,A,0); return 1;
    }
    if(s[ai]==0) {
        int k=ai,i=(k+1)%3,j=(k+2)%3;
        say("CASE: SAS / COSINES");
        say("%c^2=%c^2+%c^2-",side[k],side[i],side[j]);
        say(" 2*%c*%c*cos(%c)",side[i],side[j],angle[k]);
        say(" =%.7g^2+%.7g^2",s[i],s[j]);
        say(" -2*%.7g*%.7g",s[i],s[j]);
        say(" *cos(%.7g)",A[k]);
        double q=s[i]*s[i]+s[j]*s[j]-2*s[i]*s[j]*cs(A[k]);
        if(q<=0) { say("Numerically degenerate."); return 0; }
        s[k]=sqrt(q); say("%c=sqrt(%.7g)",side[k],q); say("%c=%.7g",side[k],s[k]);
        /* Find smaller remaining angle by cosine (avoids asin ambiguity). */
        int t=s[i]<s[j]?i:j, u=t==i?j:i;
        cosine_angle(s,A,t); missing_angle(A,u,k,t);
        finish(s,A,0); return 1;
    }
    say("CASE: SSA / SINES");
    int i=ai,j=-1,k=-1,count=0;
    for(int t=0;t<3;++t) if(t!=i) { if(s[t]>0) j=t; else k=t; }
    double r=s[j]*sn(A[i])/s[i];
    say("sin(%c)=%c*sin(%c)/%c",angle[j],side[j],angle[i],side[i]);
    say(" =%.7g*sin(%.7g)",s[j],A[i]); say(" /%.7g=%.7g",s[i],r);
    if(r>1+EPS) { say("No triangle: sin > 1."); return 0; }
    if(fabs(r-1)<EPS) r=1;
    double first=asin(clamp(r))*180/PI;
    double candidates[2]={first,180-first};
    say("%c1=asin(%.7g)",angle[j],clamp(r)); say("%c1=%.7g deg",angle[j],first);
    say("%c2=180-%.7g",angle[j],first); say("%c2=%.7g deg",angle[j],candidates[1]);
    for(int t=0;t<2;++t) {
        if(t==1 && fabs(candidates[1]-first)<1e-7) { say("Second angle duplicates"); say("the first; skip it."); continue; }
        say(" "); say("CHECK CANDIDATE %d",t+1);
        A[j]=candidates[t]; missing_angle(A,k,i,j);
        if(A[k]<=EPS) { say("Reject: last angle <=0."); continue; }
        say("Valid triangle %d",count+1);
        sine_side(s,A,k,i); finish(s,A,count); ++count;
    }
    say(" "); say("Valid solutions: %d",count); return count;
}
#ifndef HOST_TEST
static unsigned char key(void) {
    unsigned char k; do { k=os_GetCSC(); } while(!k); return k;
}
static void clear(void) { os_ClrHome(); os_SetCursorPos(0,0); }
static void row(int r,const char *s) { os_SetCursorPos(r,0); os_PutStrFull(s); }
static int digit(unsigned char k) {
    const unsigned char keys[10]={sk_0,sk_1,sk_2,sk_3,sk_4,sk_5,sk_6,sk_7,sk_8,sk_9};
    for(int i=0;i<10;++i) if(keys[i]==k) return i;
    return -1;
}
/* Numeric decimal input only: no expression evaluation. */
static int input_number(int index,double *out) {
    char b[21]="",prompt[27]; int len=0;
    clear(); row(0,"TRIANGLE: ENTER KNOWNS");
    row(1,"a opposite A, b opposite B"); row(2,"c opposite C; degrees");
    row(3,"0 / blank = unknown"); row(4,"DEL edits; CLEAR exits");
    snprintf(prompt,sizeof prompt,"%c = ?",index<3?side[index]:angle[index-3]); row(6,prompt);
    while(1) {
        char display[27]; snprintf(display,sizeof display,"%-25s",b); row(7,display);
        unsigned char k=key(); int d=digit(k);
        if(k==sk_Clear) return 0;
        if(k==sk_Enter) { *out=len?strtod(b,NULL):0; return 1; }
        if(k==sk_Del && len) b[--len]=0;
        if(d>=0 && len<19) { b[len++]=(char)('0'+d); b[len]=0; }
        if(k==sk_DecPnt && !strchr(b,'.') && len<19) { b[len++]='.'; b[len]=0; }
    }
}
static int viewer(void) {
    int page=0,pages=(nlines+7)/8;
    while(1) {
        char footer[27]; clear();
        for(int r=0;r<8;++r) if(page*8+r<nlines) row(r,lines[page*8+r]);
        snprintf(footer,sizeof footer,"Page %d/%d  UP/DOWN",page+1,pages); row(8,footer);
        row(9,"ENTER:new CLEAR:exit");
        unsigned char k=key();
        if(k==sk_Clear) return 0;
        if(k==sk_Enter) return 1;
        if((k==sk_Down || k==sk_Right) && page+1<pages) ++page;
        if((k==sk_Up || k==sk_Left) && page>0) --page;
    }
}
int main(void) {
    os_DisableHomeTextBuffer(); os_DisableCursor();
    do {
        double in[6];
        for(int i=0;i<6;++i) if(!input_number(i,&in[i])) { clear(); return 0; }
        solve(in);
    } while(viewer());
    clear(); return 0;
}
#endif
