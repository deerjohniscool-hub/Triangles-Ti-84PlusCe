#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifndef HOST_TEST
#include <ti/screen.h>
#include <ti/getcsc.h>
#endif
#define PI 3.14159265358979323846
#define EPS 1e-10
#define WIDTH 26
#define BODY 6
#define MAX_PAGES 150
typedef struct { char title[27]; char text[BODY][27]; int used; } Page;
static Page pages[MAX_PAGES];
static int npages;
static const char side[]="abc", angle[]="ABC";
static char case_name[4];
double results[2][7];
static double sn(double x){return sin(x*PI/180.0);}
static double cs(double x){return cos(x*PI/180.0);}
static double clamp(double x){return x>1?1:(x< -1?-1:x);}
static void page(const char *fmt,...){
    if(npages>=MAX_PAGES)return;
    Page *p=&pages[npages++]; memset(p,0,sizeof *p);
    va_list ap;va_start(ap,fmt);vsnprintf(p->title,sizeof p->title,fmt,ap);va_end(ap);
}
static void say(const char *fmt,...){
    char text[256];int pos=0;
    va_list ap;va_start(ap,fmt);vsnprintf(text,sizeof text,fmt,ap);va_end(ap);
    if(!npages)page("WORK");
    do{
        Page *p=&pages[npages-1];
        if(p->used==BODY){
            char title[27];strcpy(title,p->title);
            if(npages==MAX_PAGES)return;
            page("%s",title);p=&pages[npages-1];
        }
        int len=0;while(text[pos+len]&&text[pos+len]!='\n'&&len<WIDTH)++len;
        if(len==WIDTH&&text[pos+len]&&text[pos+len]!='\n')
            for(int i=len-1;i>0;--i)if(text[pos+i]==' '){len=i;break;}
        memcpy(p->text[p->used],text+pos,len);p->text[p->used++][len]=0;
        pos+=len;if(text[pos]=='\n')++pos;else while(text[pos]==' ')++pos;
    }while(text[pos]);
}
static void error(const char *a,const char *b){page("CHECK YOUR INPUT");say("%s",a);say("");say("%s",b);}
static void sine_rule(void){
    say("sin(A)/a = sin(B)/b");say("         = sin(C)/c");
}
static void missing_angle(double *A,int k,int i,int j){
    A[k]=180-A[i]-A[j];page("FIND ANGLE %c",angle[k]);
    say("Angles total 180 deg.");say("%c = 180 - %c - %c",angle[k],angle[i],angle[j]);
    say("  = 180 - %.2f",A[i]);say("        - %.2f",A[j]);say("");say("%c = %.2f deg",angle[k],A[k]);
}
static void sine_side(double *d,double *A,int k,int i){
    d[k]=d[i]*sn(A[k])/sn(A[i]);
    page("SIDE %c: LAW OF SINES",side[k]);sine_rule();say("");
    say("sin(%c)/%c = sin(%c)/%c",angle[i],side[i],angle[k],side[k]);
    page("SIDE %c: SUBSTITUTE",side[k]);
    say("sin(%.2f)/%.2f",A[i],d[i]);say(" = sin(%.2f)/%c",A[k],side[k]);say("");
    say("Cross multiply:");say("%c*sin(%.2f)",side[k],A[i]);say(" = %.2f*sin(%.2f)",d[i],A[k]);
    page("SIDE %c: SOLVE",side[k]);say("Divide by sin(%.2f):",A[i]);
    say("%c = %.2f*sin(%.2f)",side[k],d[i],A[k]);say("    / sin(%.2f)",A[i]);say("");
    say("%c = %.2f",side[k],d[k]);say("Full precision used.");
}
static void cosine_angle(double *d,double *A,int k){
    int i=(k+1)%3,j=(k+2)%3;
    double sum=d[i]*d[i]+d[j]*d[j],square=d[k]*d[k],den=2*d[i]*d[j];
    double num=sum-square,r=num/den;A[k]=acos(clamp(r))*180/PI;
    page("ANGLE %c: COSINE LAW",angle[k]);
    say("%c^2 = %c^2 + %c^2",side[k],side[i],side[j]);say("    - 2*%c*%c*cos(%c)",side[i],side[j],angle[k]);
    say("");say("Substitute the sides.");
    page("ANGLE %c: SUBSTITUTE",angle[k]);
    say("%.2f^2 = %.2f^2",d[k],d[i]);say(" + %.2f^2",d[j]);
    say(" - 2*%.2f*%.2f*cos(%c)",d[i],d[j],angle[k]);
    say("");say("%.2f = %.2f",square,sum);say(" - %.2f*cos(%c)",den,angle[k]);
    page("ANGLE %c: REARRANGE",angle[k]);
    say("Add cosine term; subtract");say("the known side squared:");
    say("%.2f*cos(%c)",den,angle[k]);say(" = %.2f - %.2f",sum,square);say(" = %.2f",num);
    page("ANGLE %c: SOLVE",angle[k]);say("Divide by %.2f:",den);
    say("cos(%c) = %.2f/%.2f",angle[k],num,den);say("cos(%c) = %.2f",angle[k],r);
    say("%c = inverse cos(above)",angle[k]);say("%c = %.2f deg",angle[k],A[k]);say("Full precision used.");
}
static void finish(double *d,double *A,int sol){
    double semi=(d[0]+d[1]+d[2])/2;
    double rad=semi*(semi-d[0])*(semi-d[1])*(semi-d[2]);
    double area=sqrt(fmax(0,rad));
    page("TRIANGLE %d: SINE AREA",sol+1);
    say("Area = (1/2)*a*b*sin(C)");say("");
    say("Area = (1/2)*%.2f",d[0]);say("       * %.2f",d[1]);say("       * sin(%.2f)",A[2]);
    page("SINE AREA: RESULT");
    say("Area = %.2f*sin(%.2f)",0.5*d[0]*d[1],A[2]);
    say("Area = %.2f sq units",0.5*d[0]*d[1]*sn(A[2]));
    say("");say("Now check using Heron.");
    page("TRIANGLE %d: HERON",sol+1);
    say("s = (a + b + c) / 2");say("  = (%.2f + %.2f",d[0],d[1]);say("     + %.2f) / 2",d[2]);
    say("");say("s = %.2f",semi);say("s means semiperimeter.");
    page("HERON: FORMULA");say("Area = sqrt(");say(" s*(s-a)*(s-b)*(s-c))");say("");
    say("s-a = %.2f",semi-d[0]);say("s-b = %.2f",semi-d[1]);say("s-c = %.2f",semi-d[2]);
    page("HERON: SUBSTITUTE");say("Area = sqrt(");say(" %.2f * %.2f",semi,semi-d[0]);
    say(" * %.2f * %.2f)",semi-d[1],semi-d[2]);say("Area = sqrt(%.2f)",fmax(0,rad));
    say("Area = %.2f sq units",area);say("Full precision used.");
    for(int i=0;i<3;++i){results[sol][i]=d[i];results[sol][i+3]=A[i];}results[sol][6]=area;
}
int solve(const double *input){
    double d[3],A[3];int ns=0,na=0,si=-1,ai=-1;
    npages=0;memset(results,0,sizeof results);strcpy(case_name,"---");page("GIVEN MEASUREMENTS");
    for(int i=0;i<3;++i){
        d[i]=input[i];A[i]=input[i+3];
        if(!isfinite(d[i])||!isfinite(A[i])||d[i]<0||A[i]<0||A[i]>=180){error("Invalid measurement.","Angles: above 0, below 180. Sides: positive.");return 0;}
        if(d[i]>0){++ns;si=i;say("%c = %.2f",side[i],d[i]);}
        if(A[i]>0){++na;ai=i;say("%c = %.2f deg",angle[i],A[i]);}
    }
    say("Angles are in degrees.");say("Displayed values rounded.");say("Math uses full precision.");
    if(ns+na!=3){error("Enter exactly 3 knowns.","Leave all others blank.");return 0;}
    if(ns==0){error("AAA gives no side scale.","Enter at least one side.");return 0;}
    if(ns==3){
        strcpy(case_name,"SSS");page("SSS: LAW OF COSINES");say("Three sides are known.");say("Find the missing angles.");
        double m=fmax(d[0],fmax(d[1],d[2]));
        if(d[0]/m+d[1]/m+d[2]/m<=2){error("No triangle possible.","Two sides must sum to more than the third.");return 0;}
        int k=0;if(d[1]>d[k])k=1;if(d[2]>d[k])k=2;
        int j=(k+1)%3,h=(k+2)%3;cosine_angle(d,A,k);cosine_angle(d,A,j);missing_angle(A,h,k,j);finish(d,A,0);return 1;
    }
    if(na==2){
        int k=0;while(A[k]>0)++k;int i=(k+1)%3,j=(k+2)%3;
        strcpy(case_name,si==k?"ASA":"AAS");page("%s: LAW OF SINES",case_name);sine_rule();
        missing_angle(A,k,i,j);
        if(A[k]<=0){error("No triangle possible.","Known angles must sum to less than 180 deg.");return 0;}
        for(int t=0;t<3;++t) { if(t!=si) sine_side(d,A,t,si); }
        finish(d,A,0);return 1;
    }
    if(d[ai]==0){
        int k=ai,i=(k+1)%3,j=(k+2)%3;strcpy(case_name,"SAS");
        page("SAS: LAW OF COSINES");say("Find the missing side,");say("then the two angles.");
        page("FIND SIDE %c",side[k]);say("%c^2 = %c^2 + %c^2",side[k],side[i],side[j]);say("    - 2*%c*%c*cos(%c)",side[i],side[j],angle[k]);
        say("  = %.2f^2 + %.2f^2",d[i],d[j]);say("    - 2*%.2f*%.2f",d[i],d[j]);say("      * cos(%.2f)",A[k]);
        double q=d[i]*d[i]+d[j]*d[j]-2*d[i]*d[j]*cs(A[k]);
        if(q<=0){error("Numerically degenerate.","Try less extreme inputs.");return 0;}
        d[k]=sqrt(q);page("SIDE %c: RESULT",side[k]);say("%c^2 = %.2f",side[k],q);say("%c = sqrt(above)",side[k]);say("%c = %.2f",side[k],d[k]);
        int t=d[i]<d[j]?i:j,u=t==i?j:i;cosine_angle(d,A,t);missing_angle(A,u,k,t);finish(d,A,0);return 1;
    }
    strcpy(case_name,"SSA");page("SSA: LAW OF SINES");sine_rule();say("Check both possible angles.");
    int i=ai,j=-1,k=-1,count=0;
    for(int t=0;t<3;++t)if(t!=i){if(d[t]>0)j=t;else k=t;}
    double r=d[j]*sn(A[i])/d[i];
    page("ANGLE %c: SINE RULE",angle[j]);say("sin(%c)/%c = sin(%c)/%c",angle[i],side[i],angle[j],side[j]);
    say("sin(%.2f)/%.2f",A[i],d[i]);say(" = sin(%c)/%.2f",angle[j],d[j]);
    page("ANGLE %c: CROSS MULTIPLY",angle[j]);say("%.2f*sin(%c)",d[i],angle[j]);say(" = %.2f*sin(%.2f)",d[j],A[i]);say("");
    say("Divide by %.2f:",d[i]);say("sin(%c) = %.2f*sin(%.2f)",angle[j],d[j],A[i]);say("         / %.2f",d[i]);
    page("ANGLE %c: SINE VALUE",angle[j]);say("sin(%c) = %.2f",angle[j],r);say("Full precision checked.");
    if(r>1+EPS){error("No triangle possible.","Sine cannot exceed 1.");return 0;}
    if(fabs(r-1)<EPS)r=1;
    double first=asin(clamp(r))*180/PI,candidates[2]={first,180-first};
    page("SSA: TWO ANGLE CHOICES");say("First: inverse sin(above)");say("%c1 = %.2f deg",angle[j],first);
    say("Second: 180 - first");say("%c2 = %.2f deg",angle[j],candidates[1]);say("Check the angle sums.");
    for(int t=0;t<2;++t){
        page("SSA: CHECK CHOICE %d",t+1);
        if(t==1&&fabs(candidates[1]-first)<1e-7){say("Same as the first angle.");say("Do not count it twice.");continue;}
        A[j]=candidates[t];say("%c = %.2f deg",angle[j],A[j]);say("Remaining angle:");say("180 - %.2f - %.2f",A[i],A[j]);
        A[k]=180-A[i]-A[j];say("%c = %.2f deg",angle[k],A[k]);
        if(A[k]<=EPS){say("REJECT: angle not > 0.");continue;}
        say("ACCEPT: triangle %d",count+1);sine_side(d,A,k,i);finish(d,A,count);++count;
    }
    page("SSA: CHECK COMPLETE");say("Valid triangles: %d",count);if(!count)say("Neither choice is valid.");return count;
}
#ifndef HOST_TEST
static unsigned char key(void){unsigned char k;do{k=os_GetCSC();}while(!k);return k;}
static void clear(void){os_ClrHome();os_SetCursorPos(0,0);}
static void row(int r,const char *s){os_SetCursorPos(r,0);os_PutStrFull(s);}
static void frame(const char *title){clear();row(0,title);row(1,"--------------------------");}
static int digit(unsigned char k){
    const unsigned char keys[10]={sk_0,sk_1,sk_2,sk_3,sk_4,sk_5,sk_6,sk_7,sk_8,sk_9};
    for(int i=0;i<10;++i)if(keys[i]==k)return i;return -1;
}
static int input_number(int index,double *out,const double *in){
    char b[21]="",buf[27];int len=0;
    snprintf(buf,sizeof buf,"ENTER %s %c  (%d/6)",index<3?"SIDE":"ANGLE",index<3?side[index]:angle[index-3],index+1);frame(buf);
    row(2,index<3?"Side opposite same letter":"Angle in DEGREES");row(3,"Blank + ENTER = unknown");row(4,"Known so far:");
    for(int r=0;r<2;++r){
        char labels[27]="";
        for(int t=r*3;t<r*3+3;++t){char part[9];snprintf(part,sizeof part,"%c:%s ",t<3?side[t]:angle[t-3],t<index&&in[t]>0?"set":"--");strncat(labels,part,sizeof labels-strlen(labels)-1);}
        row(5+r,labels);
    }
    row(8,"ENTER: Next   DEL: Edit");row(9,"CLEAR: Exit");
    while(1){
        char display[27];snprintf(display,sizeof display,"> %-23s",b);row(7,display);
        unsigned char k=key();int d=digit(k);
        if(k==sk_Clear)return 0;
        if(k==sk_Enter){*out=len?strtod(b,NULL):0;return 1;}
        if(k==sk_Del&&len)b[--len]=0;
        if(d>=0&&len<19){b[len++]=(char)('0'+d);b[len]=0;}
        if(k==sk_DecPnt&&!strchr(b,'.')&&len<19){b[len++]='.';b[len]=0;}
    }
}
static int viewer(int count){
    int pos=0,total=count+npages;
    while(1){
        char buf[80];
        if(pos<count){
            snprintf(buf,sizeof buf,"ANSWERS: TRIANGLE %d/%d",pos+1,count);frame(buf);
            snprintf(buf,sizeof buf,"Case: %s   Degrees",case_name);row(2,buf);row(3,"Sides          Angles");
            for(int i=0;i<3;++i){
                snprintf(buf,sizeof buf,"%c=%-9.2f %c=%.2f",side[i],results[pos][i],angle[i],results[pos][i+3]);
                if(strlen(buf)>26)strcpy(buf,"Large values: see work");row(4+i,buf);
            }
            snprintf(buf,sizeof buf,"Area = %.2f",results[pos][6]);if(strlen(buf)>26)strcpy(buf,"Area: see work pages");row(7,buf);
        }else{Page *p=&pages[pos-count];frame(p->title);for(int r=0;r<p->used;++r)row(r+2,p->text[r]);}
        snprintf(buf,sizeof buf,"%d/%d  LEFT/RIGHT: Pages",pos+1,total);row(8,buf);row(9,"ENTER: New   CLEAR: Exit");
        unsigned char k=key();if(k==sk_Clear)return 0;if(k==sk_Enter)return 1;
        if((k==sk_Down||k==sk_Right)&&pos+1<total)++pos;
        if((k==sk_Up||k==sk_Left)&&pos>0)--pos;
    }
}
int main(void){
    os_DisableHomeTextBuffer();os_DisableCursor();
    do{
        double in[6]={0};
        for(int i=0;i<6;++i)if(!input_number(i,&in[i],in)){clear();return 0;}
        int count=solve(in);
        for(int j=0;j<count;++j){
            page("TRIANGLE %d: FULL ANSWERS",j+1);
            for(int i=0;i<3;++i){say("%c = %.2f",side[i],results[j][i]);say("%c = %.2f deg",angle[i],results[j][i+3]);}
            say("Area = %.2f sq units",results[j][6]);
        }
        if(!viewer(count))break;
    }while(1);clear();return 0;
}
#endif
