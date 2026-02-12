/**
 * @file yscanf3.h
 * @brief High-performance buffered input parser (EOF-correct version)
 * @author Summer PLUS Studio
 * @email yuzhouhunter@outlook.com
 * @version 3.0
 */

#ifndef YSCANF3_H
#define YSCANF3_H

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

/* ========================= CONFIG ========================= */

#ifndef YSCANF_BUFFER_SIZE
#define YSCANF_BUFFER_SIZE (1 << 22)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define YLIKELY(x)   __builtin_expect(!!(x),1)
#define YUNLIKELY(x) __builtin_expect(!!(x),0)
#else
#define YLIKELY(x)   (x)
#define YUNLIKELY(x) (x)
#endif

/* ========================= BUFFER ========================= */

static char ybuf[YSCANF_BUFFER_SIZE];
static char *yptr=ybuf,*yend=ybuf;
static int yeof=0;

/* ========================= CORE IO ========================= */

static inline int yget(void)
{
	if(yptr>=yend){
		if(yeof)return EOF;
		size_t len=fread(ybuf,1,sizeof(ybuf),stdin);
		if(!len){yeof=1;return EOF;}
		yptr=ybuf;
		yend=ybuf+len;
	}
	return (unsigned char)*yptr++;
}

static inline int ypeek(void)
{
	if(yptr>=yend){
		if(yeof)return EOF;
		size_t len=fread(ybuf,1,sizeof(ybuf),stdin);
		if(!len){yeof=1;return EOF;}
		yptr=ybuf;
		yend=ybuf+len;
	}
	return (unsigned char)*yptr;
}

static inline void yskip_space(void)
{
	int c;
	while((c=ypeek())!=EOF&&isspace(c))yget();
}

/* ========================= READERS ========================= */

static inline int yread_int_ok(int *out)
{
	int c,sign=1;
	long long x=0;
	yskip_space();
	c=ypeek();
	if(c==EOF)return 0;
	if(c=='+'||c=='-'){
		sign=(c=='-')?-1:1;
		yget();
	}
	c=ypeek();
	if(c<'0'||c>'9')return 0;
	while((c=ypeek())>='0'&&c<='9'){
		x=x*10+(yget()-'0');
	}
	*out=(int)(x*sign);
	return 1;
}

static inline int yread_uint_ok(unsigned *out)
{
	int c;
	unsigned long long x=0;
	yskip_space();
	c=ypeek();
	if(c==EOF||c<'0'||c>'9')return 0;
	while((c=ypeek())>='0'&&c<='9'){
		x=x*10+(yget()-'0');
	}
	*out=(unsigned)x;
	return 1;
}

static inline int yread_double_ok(double *out)
{
	int c,sign=1;
	double x=0,frac=0,base=1;
	yskip_space();
	c=ypeek();
	if(c==EOF)return 0;
	if(c=='+'||c=='-'){
		sign=(c=='-')?-1:1;
		yget();
	}
	c=ypeek();
	if((c<'0'||c>'9')&&c!='.')return 0;
	while((c=ypeek())>='0'&&c<='9'){
		x=x*10+(yget()-'0');
	}
	if(ypeek()=='.'){
		yget();
		while((c=ypeek())>='0'&&c<='9'){
			frac=frac*10+(yget()-'0');
			base*=10;
		}
	}
	x=sign*(x+frac/base);
	if((c=ypeek())=='e'||c=='E'){
		yget();
		int es=1,ep=0;
		c=ypeek();
		if(c=='+'||c=='-'){
			es=(c=='-')?-1:1;
			yget();
		}
		while((c=ypeek())>='0'&&c<='9'){
			ep=ep*10+(yget()-'0');
		}
		double p=1;
		while(ep--)p*=10;
		if(es<0)x/=p;
		else x*=p;
	}
	*out=x;
	return 1;
}

static inline int yread_str_ok(char *s)
{
	int c;
	yskip_space();
	c=ypeek();
	if(c==EOF)return 0;
	while((c=ypeek())!=EOF&&!isspace(c)){
		*s++=yget();
	}
	*s=0;
	return 1;
}

static inline int yread_line_ok(char *s,int maxlen)
{
	int c,len=0;

	while(1){
		c=yget();
		if(c==EOF)return 0;
		if(c=='\n'||c=='\r')continue;
		while(c!=EOF&&c!='\n'&&c!='\r'){
			if(len<maxlen-1)
				s[len++]=(char)c;
			c=yget();
		}
		if(c=='\r'&&ypeek()=='\n')
			yget();
		s[len]=0;
		return 1;
	}
}

static inline int ygetline_ok(char *s,int maxlen)
{
	int c,len=0;
	c=yget();
	if(c==EOF)return 0;
	while(c!=EOF&&c!='\n'&&c!='\r'){
		if(len<maxlen-1)
			s[len++]=(char)c;
		c=yget();
	}
	if(c=='\r'&&ypeek()=='\n')
		yget();
	s[len]=0;
	return 1;
}

static inline int yread_ll_ok(long long *out)
{
	int c,sign=1;
	long long x=0;
	yskip_space();
	c=ypeek();
	if(c==EOF)return 0;
	if(c=='+'||c=='-'){
		sign=(c=='-')?-1:1;
		yget();
	}
	c=ypeek();
	if(c<'0'||c>'9')return 0;
	while((c=ypeek())>='0'&&c<='9'){
		x=x*10+(yget()-'0');
	}
	*out=x*sign;
	return 1;
}

static inline int yread_ull_ok(unsigned long long *out)
{
	int c;
	unsigned long long x=0;
	yskip_space();
	c=ypeek();
	if(c==EOF||c<'0'||c>'9')return 0;
	while((c=ypeek())>='0'&&c<='9'){
		x=x*10+(yget()-'0');
	}
	*out=x;
	return 1;
}

/* ========================= YSCANF ========================= */

static inline int yscanf(const char *fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	int cnt=0;

	while(*fmt){
		if(isspace(*fmt)){
			yskip_space();
			fmt++;
			continue;
		}
		if(*fmt!='%'){
			fmt++;
			continue;
		}
		fmt++;

		if(*fmt=='d'){
			int *p=va_arg(ap,int*);
			if(!yread_int_ok(p)){va_end(ap);return cnt?cnt:EOF;}
			cnt++;
		}
		else if(*fmt=='u'){
			unsigned *p=va_arg(ap,unsigned*);
			if(!yread_uint_ok(p)){va_end(ap);return cnt?cnt:EOF;}
			cnt++;
		}
		else if(*fmt=='l'){
			fmt++;
			if(*fmt=='l'){
				fmt++;
				if(*fmt=='d'){
					long long *p=va_arg(ap,long long*);
					if(!yread_ll_ok(p)){va_end(ap);return cnt?cnt:EOF;}
					cnt++;
				}
				else if(*fmt=='u'){
					unsigned long long *p=va_arg(ap,unsigned long long*);
					if(!yread_ull_ok(p)){va_end(ap);return cnt?cnt:EOF;}
					cnt++;
				}
				else{
					va_end(ap);
					return -1;
				}
			}
			else{
				va_end(ap);
				return -1;
			}
		}
		else if(*fmt=='f'||*fmt=='e'||*fmt=='g'){
			double *p=va_arg(ap,double*);
			if(!yread_double_ok(p)){va_end(ap);return cnt?cnt:EOF;}
			cnt++;
		}
		else if(*fmt=='s'){
			char *p=va_arg(ap,char*);
			if(!yread_str_ok(p)){va_end(ap);return cnt?cnt:EOF;}
			cnt++;
		}
		else if(*fmt=='c'){
			char *p=va_arg(ap,char*);
			int c=yget();
			if(c==EOF){va_end(ap);return cnt?cnt:EOF;}
			*p=(char)c;
			cnt++;
		}
		else{
			va_end(ap);
			return -1;
		}
		fmt++;
	}

	va_end(ap);
	return cnt;
}

#endif /* YSCANF3_H */
