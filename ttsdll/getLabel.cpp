#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getLabel.h"

PinyinInfo pinyin2phone[411];

/*
	二分查找数组中的 pinyin 对应的索引 
*/
short getPinyinInfoID(char *pinyin)
{
    short l=0,u=411;
    short m,comp;
    while(u-l>=2)
	{
        m=(u+l)/2;
        comp=strcmp(pinyin,pinyin2phone[m].pinyin);
        if(comp==0){return m;}
        else if(comp<0){u=m;}
        else{(l=m);}
    }
    if(strcmp(pinyin,pinyin2phone[l].pinyin)==0){
        return l;
    }
    if(strcmp(pinyin,pinyin2phone[u].pinyin)==0){
        return u;
    }
    return -1;
}

/*
	获取这个pinyin对应的：声母 韵母 音调 
	pinyin 的最后一位就是yindiao 
*/
void getShengYun(char *pinyin,char *shengmu,char *yunmu,short *yindiao)
{
    PinyinInfo *pif;//
    char py[9];
    int len=strlen(pinyin);
    int pyID;

    if(pinyin[len-1]=='1'){*yindiao=1;}
    if(pinyin[len-1]=='2'){*yindiao=2;}
    if(pinyin[len-1]=='3'){*yindiao=3;}
    if(pinyin[len-1]=='4'){*yindiao=4;}
    if(pinyin[len-1]=='5'){*yindiao=5;}

    strcpy(py,pinyin);
    py[len-1]=0;


    pyID = getPinyinInfoID(py);
    pif = pinyin2phone+pyID;
    if(pyID<0)
	{
        printf("warning: pinyin %s does not exist!\n",py);
    }
    strcpy(shengmu,pif->shengmu);
    strcpy(yunmu,pif->yunmu);
};

/*
	char **pinyinSeq: sNum 个拼音
	short *tag :	   sNum 个pos标记(0123其他)  0表示韵律词内部 句尾默认是4 
	最后得到label序列需要的信息 存储在 cif中 
*/
void TtsLabel_ObtainLabelCharSeq(TtsLabelCharInfo * cif,char **pinyinSeq,short sNum,short * tag)
{
    //tag: 0,1,2
    int i;

	// 第一个词  从后往前数 
    cif[0].CharInPwNum=-1;
    cif[0].CharInPpNum=-1;
    cif[0].CharInIpNum=-1;
    cif[0].CharInSentNum=-1;
    cif[0].PwInPpNum=-1;
    cif[0].PwInIpNum=-1;
    cif[0].PwInSentNum=-1;
    cif[0].PpInIpNum=-1;
    cif[0].PpInSentNum=-1;
    cif[0].IpInSentNum=-1;
	// 从前往后 ?
    cif[0].CharInPwPos=1;
    cif[0].CharInPpPos=1;
    cif[0].CharInIpPos=1;
    cif[0].CharInSentPos=1;
    cif[0].PwInPpPos=1;
    cif[0].PwInIpPos=1;
    cif[0].PwInSentPos=1;
    cif[0].PpInIpPos=1;
    cif[0].PpInSentPos=1;
    cif[0].IpInSentPos=1;

	// 获取每个pinyin的 声韵母 yindiao 等 
    for(i=0;i<sNum;i++)
	{
        strcpy(cif[i].pinyin,pinyinSeq[i]);
        getShengYun(pinyinSeq[i], cif[i].shengmu, cif[i].yunmu, &(cif[i].yindiao));
    }

	// 除了第一个词外 后面每个字 
    for(i=1;i<sNum;i++)
	{
		// 当前char和上一个char 属于一个韵律词  
        if(tag[i-1]==0)
		{
			// 新的char 
            cif[i].CharInPwPos		= cif[i-1].CharInPwPos+1;
            cif[i].CharInPpPos		= cif[i-1].CharInPpPos+1;
            cif[i].CharInIpPos		= cif[i-1].CharInIpPos+1;
            cif[i].CharInSentPos	= i+1;
			// 韵律词 信息  与 i-1 相同 
            cif[i].PwInPpPos	=	cif[i-1].PwInPpPos;
            cif[i].PwInIpPos	=	cif[i-1].PwInIpPos;
            cif[i].PwInSentPos	=	cif[i-1].PwInSentPos;
            cif[i].PpInIpPos	=	cif[i-1].PpInIpPos;
            cif[i].PpInSentPos	=	cif[i-1].PpInSentPos;
            cif[i].IpInSentPos	=	cif[i-1].IpInSentPos;
        }
		// 前一个词处  是韵律词划分
		// 当前char 是新的韵律词开始   
        else if(tag[i-1]==1)
		{
			// 新的char 
            cif[i].CharInPwPos	=1;
            cif[i].CharInPpPos	=cif[i-1].CharInPpPos+1;
            cif[i].CharInIpPos	=cif[i-1].CharInIpPos+1;
            cif[i].CharInSentPos=i+1;
			// 新的韵律词 
            cif[i].PwInPpPos	=	cif[i-1].PwInPpPos+1;
            cif[i].PwInIpPos	=	cif[i-1].PwInIpPos+1;
            cif[i].PwInSentPos	=	cif[i-1].PwInSentPos+1;
			// 同一个韵律短语和语调短语 
            cif[i].PpInIpPos	=	cif[i-1].PpInIpPos;
            cif[i].PpInSentPos	=	cif[i-1].PpInSentPos;
            cif[i].IpInSentPos	=	cif[i-1].IpInSentPos;

        }
		// 新的 韵律短语 pp  
        else if(tag[i-1]==2)
		{
			// 新 char 
            cif[i].CharInPwPos=1;
            cif[i].CharInPpPos=1;
            cif[i].CharInIpPos=cif[i-1].CharInIpPos+1;
            cif[i].CharInSentPos=i+1;
			// 新 韵律词开始  pw
            cif[i].PwInPpPos	=	1;
            cif[i].PwInIpPos	=	cif[i-1].PwInIpPos+1;
            cif[i].PwInSentPos	=	cif[i-1].PwInSentPos+1;
			// 新 韵律短语开始 pp 
            cif[i].PpInIpPos=cif[i-1].PpInIpPos+1;
            cif[i].PpInSentPos=cif[i-1].PpInSentPos+1;
            cif[i].IpInSentPos=cif[i-1].IpInSentPos;
        }
		// 3或4 新语调短语 开始 ip 
        else
		{
			// 新 char 
            cif[i].CharInPwPos=1;
            cif[i].CharInPpPos=1;
            cif[i].CharInIpPos=1;
            cif[i].CharInSentPos=i+1;
			// 新pw 
            cif[i].PwInPpPos=1;
            cif[i].PwInIpPos=1;
            cif[i].PwInSentPos=cif[i-1].PwInSentPos+1;
            cif[i].PpInIpPos=1;
            cif[i].PpInSentPos=cif[i-1].PpInSentPos+1;
            cif[i].IpInSentPos=cif[i-1].IpInSentPos+1;
        }
    }

    for(i=0;i<sNum;i++)
	{
        if(tag[i]==0){
            cif[i].CharInPwNum=-1;
            cif[i].CharInPpNum=-1;
            cif[i].CharInIpNum=-1;
            cif[i].CharInSentNum=-1;
            cif[i].PwInPpNum=-1;
            cif[i].PwInIpNum=-1;
            cif[i].PwInSentNum=-1;
            cif[i].PpInIpNum=-1;
            cif[i].PpInSentNum=-1;
            cif[i].IpInSentNum=-1;
        }
        else if(tag[i]==1){
            cif[i].CharInPwNum=cif[i].CharInPwPos;
            cif[i].CharInPpNum=-1;
            cif[i].CharInIpNum=-1;
            cif[i].CharInSentNum=-1;
            cif[i].PwInPpNum=-1;
            cif[i].PwInIpNum=-1;
            cif[i].PwInSentNum=-1;
            cif[i].PpInIpNum=-1;
            cif[i].PpInSentNum=-1;
            cif[i].IpInSentNum=-1;
        }
        else if(tag[i]==2){
            cif[i].CharInPwNum=cif[i].CharInPwPos;
            cif[i].CharInPpNum=cif[i].CharInPpPos;
            cif[i].CharInIpNum=-1;
            cif[i].CharInSentNum=-1;
            cif[i].PwInPpNum=cif[i].PwInPpPos;
            cif[i].PwInIpNum=-1;
            cif[i].PwInSentNum=-1;
            cif[i].PpInIpNum=-1;
            cif[i].PpInSentNum=-1;
            cif[i].IpInSentNum=-1;
        }
        else if(tag[i]==3){
            cif[i].CharInPwNum=cif[i].CharInPwPos;
            cif[i].CharInPpNum=cif[i].CharInPpPos;
            cif[i].CharInIpNum=cif[i].CharInIpPos;
            cif[i].CharInSentNum=-1;
            cif[i].PwInPpNum=cif[i].PwInPpPos;
            cif[i].PwInIpNum=cif[i].PwInIpPos;
            cif[i].PwInSentNum=-1;
            cif[i].PpInIpNum=cif[i].PpInIpPos;
            cif[i].PpInSentNum=-1;
            cif[i].IpInSentNum=-1;
        }
        else{
            cif[i].CharInPwNum=cif[i].CharInPwPos;
            cif[i].CharInPpNum=cif[i].CharInPpPos;
            cif[i].CharInIpNum=cif[i].CharInIpPos;
            cif[i].CharInSentNum=cif[i].CharInSentPos;
            cif[i].PwInPpNum=cif[i].PwInPpPos;
            cif[i].PwInIpNum=cif[i].PwInIpPos;
            cif[i].PwInSentNum=cif[i].PwInSentPos;
            cif[i].PpInIpNum=cif[i].PpInIpPos;
            cif[i].PpInSentNum=cif[i].PpInSentPos;
            cif[i].IpInSentNum=cif[i].IpInSentPos;
        }
    }

    //for(i=0;i<7;i++){
      //  printf("%d %d %d %d\n",cif[i].CharInPwNum,cif[i].CharInPpNum,cif[i].PwInPpNum,cif[i].PwInSentNum);
    //}
    for(i=sNum-2;i>=0;i--){
        if(cif[i].CharInPwNum<0){cif[i].CharInPwNum=cif[i+1].CharInPwNum;}
        if(cif[i].CharInPpNum<0){cif[i].CharInPpNum=cif[i+1].CharInPpNum;}
        if(cif[i].CharInIpNum<0){cif[i].CharInIpNum=cif[i+1].CharInIpNum;}
        if(cif[i].CharInSentNum<0){cif[i].CharInSentNum=cif[i+1].CharInSentNum;}
        if(cif[i].PwInPpNum<0){cif[i].PwInPpNum=cif[i+1].PwInPpNum;}
        if(cif[i].PwInIpNum<0){cif[i].PwInIpNum=cif[i+1].PwInIpNum;}
        if(cif[i].PwInSentNum<0){cif[i].PwInSentNum=cif[i+1].PwInSentNum;}
        if(cif[i].PpInIpNum<0){cif[i].PpInIpNum=cif[i+1].PpInIpNum;}
        if(cif[i].PpInSentNum<0){cif[i].PpInSentNum=cif[i+1].PpInSentNum;}
        if(cif[i].IpInSentNum<0){cif[i].IpInSentNum=cif[i+1].IpInSentNum;}
    }

}

int isQing(char *phone){
    char *qlist[]={"zh","sil","ch","c","b","d","g","f","h","k","j","q","p","s","sh","t","x","z"};
    int i;
    for(i=0;i<18;i++){
        if(strcmp(phone,qlist[i])==0){
            return 1;
        }
    }
    return 0;
}

/*
	初始化 410个音节 对应的音素拆分 
	strcpy(pinyin2phone[189].pinyin,"lve");
	strcpy(pinyin2phone[189].shengmu,"l");
	strcpy(pinyin2phone[189].yunmu,"uxe");
*/
void TTS_Label_Init(){
	strcpy(pinyin2phone[0].pinyin,"a");
	strcpy(pinyin2phone[0].shengmu,"as");
	strcpy(pinyin2phone[0].yunmu,"a");
	strcpy(pinyin2phone[1].pinyin,"ai");
	strcpy(pinyin2phone[1].shengmu,"as");
	strcpy(pinyin2phone[1].yunmu,"ai");
	strcpy(pinyin2phone[2].pinyin,"an");
	strcpy(pinyin2phone[2].shengmu,"as");
	strcpy(pinyin2phone[2].yunmu,"an");
	strcpy(pinyin2phone[3].pinyin,"ang");
	strcpy(pinyin2phone[3].shengmu,"as");
	strcpy(pinyin2phone[3].yunmu,"ang");
	strcpy(pinyin2phone[4].pinyin,"ao");
	strcpy(pinyin2phone[4].shengmu,"as");
	strcpy(pinyin2phone[4].yunmu,"ao");
	strcpy(pinyin2phone[5].pinyin,"ba");
	strcpy(pinyin2phone[5].shengmu,"b");
	strcpy(pinyin2phone[5].yunmu,"a");
	strcpy(pinyin2phone[6].pinyin,"bai");
	strcpy(pinyin2phone[6].shengmu,"b");
	strcpy(pinyin2phone[6].yunmu,"ai");
	strcpy(pinyin2phone[7].pinyin,"ban");
	strcpy(pinyin2phone[7].shengmu,"b");
	strcpy(pinyin2phone[7].yunmu,"an");
	strcpy(pinyin2phone[8].pinyin,"bang");
	strcpy(pinyin2phone[8].shengmu,"b");
	strcpy(pinyin2phone[8].yunmu,"ang");
	strcpy(pinyin2phone[9].pinyin,"bao");
	strcpy(pinyin2phone[9].shengmu,"b");
	strcpy(pinyin2phone[9].yunmu,"ao");
	strcpy(pinyin2phone[10].pinyin,"bei");
	strcpy(pinyin2phone[10].shengmu,"b");
	strcpy(pinyin2phone[10].yunmu,"ei");
	strcpy(pinyin2phone[11].pinyin,"ben");
	strcpy(pinyin2phone[11].shengmu,"b");
	strcpy(pinyin2phone[11].yunmu,"en");
	strcpy(pinyin2phone[12].pinyin,"beng");
	strcpy(pinyin2phone[12].shengmu,"b");
	strcpy(pinyin2phone[12].yunmu,"eng");
	strcpy(pinyin2phone[13].pinyin,"bi");
	strcpy(pinyin2phone[13].shengmu,"b");
	strcpy(pinyin2phone[13].yunmu,"i");
	strcpy(pinyin2phone[14].pinyin,"bian");
	strcpy(pinyin2phone[14].shengmu,"b");
	strcpy(pinyin2phone[14].yunmu,"ian");
	strcpy(pinyin2phone[15].pinyin,"biao");
	strcpy(pinyin2phone[15].shengmu,"b");
	strcpy(pinyin2phone[15].yunmu,"iao");
	strcpy(pinyin2phone[16].pinyin,"bie");
	strcpy(pinyin2phone[16].shengmu,"b");
	strcpy(pinyin2phone[16].yunmu,"ie");
	strcpy(pinyin2phone[17].pinyin,"bin");
	strcpy(pinyin2phone[17].shengmu,"b");
	strcpy(pinyin2phone[17].yunmu,"in");
	strcpy(pinyin2phone[18].pinyin,"bing");
	strcpy(pinyin2phone[18].shengmu,"b");
	strcpy(pinyin2phone[18].yunmu,"ing");
	strcpy(pinyin2phone[19].pinyin,"bo");
	strcpy(pinyin2phone[19].shengmu,"b");
	strcpy(pinyin2phone[19].yunmu,"o");
	strcpy(pinyin2phone[20].pinyin,"bu");
	strcpy(pinyin2phone[20].shengmu,"b");
	strcpy(pinyin2phone[20].yunmu,"u");
	strcpy(pinyin2phone[21].pinyin,"ca");
	strcpy(pinyin2phone[21].shengmu,"c");
	strcpy(pinyin2phone[21].yunmu,"a");
	strcpy(pinyin2phone[22].pinyin,"cai");
	strcpy(pinyin2phone[22].shengmu,"c");
	strcpy(pinyin2phone[22].yunmu,"ai");
	strcpy(pinyin2phone[23].pinyin,"can");
	strcpy(pinyin2phone[23].shengmu,"c");
	strcpy(pinyin2phone[23].yunmu,"an");
	strcpy(pinyin2phone[24].pinyin,"cang");
	strcpy(pinyin2phone[24].shengmu,"c");
	strcpy(pinyin2phone[24].yunmu,"ang");
	strcpy(pinyin2phone[25].pinyin,"cao");
	strcpy(pinyin2phone[25].shengmu,"c");
	strcpy(pinyin2phone[25].yunmu,"ao");
	strcpy(pinyin2phone[26].pinyin,"ce");
	strcpy(pinyin2phone[26].shengmu,"c");
	strcpy(pinyin2phone[26].yunmu,"e");
	strcpy(pinyin2phone[27].pinyin,"cen");
	strcpy(pinyin2phone[27].shengmu,"c");
	strcpy(pinyin2phone[27].yunmu,"en");
	strcpy(pinyin2phone[28].pinyin,"ceng");
	strcpy(pinyin2phone[28].shengmu,"c");
	strcpy(pinyin2phone[28].yunmu,"eng");
	strcpy(pinyin2phone[29].pinyin,"cha");
	strcpy(pinyin2phone[29].shengmu,"ch");
	strcpy(pinyin2phone[29].yunmu,"a");
	strcpy(pinyin2phone[30].pinyin,"chai");
	strcpy(pinyin2phone[30].shengmu,"ch");
	strcpy(pinyin2phone[30].yunmu,"ai");
	strcpy(pinyin2phone[31].pinyin,"chan");
	strcpy(pinyin2phone[31].shengmu,"ch");
	strcpy(pinyin2phone[31].yunmu,"an");
	strcpy(pinyin2phone[32].pinyin,"chang");
	strcpy(pinyin2phone[32].shengmu,"ch");
	strcpy(pinyin2phone[32].yunmu,"ang");
	strcpy(pinyin2phone[33].pinyin,"chao");
	strcpy(pinyin2phone[33].shengmu,"ch");
	strcpy(pinyin2phone[33].yunmu,"ao");
	strcpy(pinyin2phone[34].pinyin,"che");
	strcpy(pinyin2phone[34].shengmu,"ch");
	strcpy(pinyin2phone[34].yunmu,"e");
	strcpy(pinyin2phone[35].pinyin,"chen");
	strcpy(pinyin2phone[35].shengmu,"ch");
	strcpy(pinyin2phone[35].yunmu,"en");
	strcpy(pinyin2phone[36].pinyin,"cheng");
	strcpy(pinyin2phone[36].shengmu,"ch");
	strcpy(pinyin2phone[36].yunmu,"eng");
	strcpy(pinyin2phone[37].pinyin,"chi");
	strcpy(pinyin2phone[37].shengmu,"ch");
	strcpy(pinyin2phone[37].yunmu,"ib");
	strcpy(pinyin2phone[38].pinyin,"chong");
	strcpy(pinyin2phone[38].shengmu,"ch");
	strcpy(pinyin2phone[38].yunmu,"ong");
	strcpy(pinyin2phone[39].pinyin,"chou");
	strcpy(pinyin2phone[39].shengmu,"ch");
	strcpy(pinyin2phone[39].yunmu,"ou");
	strcpy(pinyin2phone[40].pinyin,"chu");
	strcpy(pinyin2phone[40].shengmu,"ch");
	strcpy(pinyin2phone[40].yunmu,"u");
	strcpy(pinyin2phone[41].pinyin,"chua");
	strcpy(pinyin2phone[41].shengmu,"ch");
	strcpy(pinyin2phone[41].yunmu,"ua");
	strcpy(pinyin2phone[42].pinyin,"chuai");
	strcpy(pinyin2phone[42].shengmu,"ch");
	strcpy(pinyin2phone[42].yunmu,"uai");
	strcpy(pinyin2phone[43].pinyin,"chuan");
	strcpy(pinyin2phone[43].shengmu,"ch");
	strcpy(pinyin2phone[43].yunmu,"uan");
	strcpy(pinyin2phone[44].pinyin,"chuang");
	strcpy(pinyin2phone[44].shengmu,"ch");
	strcpy(pinyin2phone[44].yunmu,"uang");
	strcpy(pinyin2phone[45].pinyin,"chui");
	strcpy(pinyin2phone[45].shengmu,"ch");
	strcpy(pinyin2phone[45].yunmu,"ui");
	strcpy(pinyin2phone[46].pinyin,"chun");
	strcpy(pinyin2phone[46].shengmu,"ch");
	strcpy(pinyin2phone[46].yunmu,"un");
	strcpy(pinyin2phone[47].pinyin,"chuo");
	strcpy(pinyin2phone[47].shengmu,"ch");
	strcpy(pinyin2phone[47].yunmu,"uo");
	strcpy(pinyin2phone[48].pinyin,"ci");
	strcpy(pinyin2phone[48].shengmu,"c");
	strcpy(pinyin2phone[48].yunmu,"if");
	strcpy(pinyin2phone[49].pinyin,"cong");
	strcpy(pinyin2phone[49].shengmu,"c");
	strcpy(pinyin2phone[49].yunmu,"ong");
	strcpy(pinyin2phone[50].pinyin,"cou");
	strcpy(pinyin2phone[50].shengmu,"c");
	strcpy(pinyin2phone[50].yunmu,"ou");
	strcpy(pinyin2phone[51].pinyin,"cu");
	strcpy(pinyin2phone[51].shengmu,"c");
	strcpy(pinyin2phone[51].yunmu,"u");
	strcpy(pinyin2phone[52].pinyin,"cuan");
	strcpy(pinyin2phone[52].shengmu,"c");
	strcpy(pinyin2phone[52].yunmu,"uan");
	strcpy(pinyin2phone[53].pinyin,"cui");
	strcpy(pinyin2phone[53].shengmu,"c");
	strcpy(pinyin2phone[53].yunmu,"ui");
	strcpy(pinyin2phone[54].pinyin,"cun");
	strcpy(pinyin2phone[54].shengmu,"c");
	strcpy(pinyin2phone[54].yunmu,"un");
	strcpy(pinyin2phone[55].pinyin,"cuo");
	strcpy(pinyin2phone[55].shengmu,"c");
	strcpy(pinyin2phone[55].yunmu,"uo");
	strcpy(pinyin2phone[56].pinyin,"da");
	strcpy(pinyin2phone[56].shengmu,"d");
	strcpy(pinyin2phone[56].yunmu,"a");
	strcpy(pinyin2phone[57].pinyin,"dai");
	strcpy(pinyin2phone[57].shengmu,"d");
	strcpy(pinyin2phone[57].yunmu,"ai");
	strcpy(pinyin2phone[58].pinyin,"dan");
	strcpy(pinyin2phone[58].shengmu,"d");
	strcpy(pinyin2phone[58].yunmu,"an");
	strcpy(pinyin2phone[59].pinyin,"dang");
	strcpy(pinyin2phone[59].shengmu,"d");
	strcpy(pinyin2phone[59].yunmu,"ang");
	strcpy(pinyin2phone[60].pinyin,"dao");
	strcpy(pinyin2phone[60].shengmu,"d");
	strcpy(pinyin2phone[60].yunmu,"ao");
	strcpy(pinyin2phone[61].pinyin,"de");
	strcpy(pinyin2phone[61].shengmu,"d");
	strcpy(pinyin2phone[61].yunmu,"e");
	strcpy(pinyin2phone[62].pinyin,"dei");
	strcpy(pinyin2phone[62].shengmu,"d");
	strcpy(pinyin2phone[62].yunmu,"ei");
	strcpy(pinyin2phone[63].pinyin,"den");
	strcpy(pinyin2phone[63].shengmu,"d");
	strcpy(pinyin2phone[63].yunmu,"en");
	strcpy(pinyin2phone[64].pinyin,"deng");
	strcpy(pinyin2phone[64].shengmu,"d");
	strcpy(pinyin2phone[64].yunmu,"eng");
	strcpy(pinyin2phone[65].pinyin,"di");
	strcpy(pinyin2phone[65].shengmu,"d");
	strcpy(pinyin2phone[65].yunmu,"i");
	strcpy(pinyin2phone[66].pinyin,"dia");
	strcpy(pinyin2phone[66].shengmu,"d");
	strcpy(pinyin2phone[66].yunmu,"ia");
	strcpy(pinyin2phone[67].pinyin,"dian");
	strcpy(pinyin2phone[67].shengmu,"d");
	strcpy(pinyin2phone[67].yunmu,"ian");
	strcpy(pinyin2phone[68].pinyin,"diao");
	strcpy(pinyin2phone[68].shengmu,"d");
	strcpy(pinyin2phone[68].yunmu,"iao");
	strcpy(pinyin2phone[69].pinyin,"die");
	strcpy(pinyin2phone[69].shengmu,"d");
	strcpy(pinyin2phone[69].yunmu,"ie");
	strcpy(pinyin2phone[70].pinyin,"ding");
	strcpy(pinyin2phone[70].shengmu,"d");
	strcpy(pinyin2phone[70].yunmu,"ing");
	strcpy(pinyin2phone[71].pinyin,"diu");
	strcpy(pinyin2phone[71].shengmu,"d");
	strcpy(pinyin2phone[71].yunmu,"iu");
	strcpy(pinyin2phone[72].pinyin,"dong");
	strcpy(pinyin2phone[72].shengmu,"d");
	strcpy(pinyin2phone[72].yunmu,"ong");
	strcpy(pinyin2phone[73].pinyin,"dou");
	strcpy(pinyin2phone[73].shengmu,"d");
	strcpy(pinyin2phone[73].yunmu,"ou");
	strcpy(pinyin2phone[74].pinyin,"du");
	strcpy(pinyin2phone[74].shengmu,"d");
	strcpy(pinyin2phone[74].yunmu,"u");
	strcpy(pinyin2phone[75].pinyin,"duan");
	strcpy(pinyin2phone[75].shengmu,"d");
	strcpy(pinyin2phone[75].yunmu,"uan");
	strcpy(pinyin2phone[76].pinyin,"dui");
	strcpy(pinyin2phone[76].shengmu,"d");
	strcpy(pinyin2phone[76].yunmu,"ui");
	strcpy(pinyin2phone[77].pinyin,"dun");
	strcpy(pinyin2phone[77].shengmu,"d");
	strcpy(pinyin2phone[77].yunmu,"un");
	strcpy(pinyin2phone[78].pinyin,"duo");
	strcpy(pinyin2phone[78].shengmu,"d");
	strcpy(pinyin2phone[78].yunmu,"uo");
	strcpy(pinyin2phone[79].pinyin,"e");
	strcpy(pinyin2phone[79].shengmu,"es");
	strcpy(pinyin2phone[79].yunmu,"e");
	strcpy(pinyin2phone[80].pinyin,"ei");
	strcpy(pinyin2phone[80].shengmu,"es");
	strcpy(pinyin2phone[80].yunmu,"ei");
	strcpy(pinyin2phone[81].pinyin,"en");
	strcpy(pinyin2phone[81].shengmu,"es");
	strcpy(pinyin2phone[81].yunmu,"en");
	strcpy(pinyin2phone[82].pinyin,"eng");
	strcpy(pinyin2phone[82].shengmu,"es");
	strcpy(pinyin2phone[82].yunmu,"eng");
	strcpy(pinyin2phone[83].pinyin,"er");
	strcpy(pinyin2phone[83].shengmu,"es");
	strcpy(pinyin2phone[83].yunmu,"er");
	strcpy(pinyin2phone[84].pinyin,"fa");
	strcpy(pinyin2phone[84].shengmu,"f");
	strcpy(pinyin2phone[84].yunmu,"a");
	strcpy(pinyin2phone[85].pinyin,"fan");
	strcpy(pinyin2phone[85].shengmu,"f");
	strcpy(pinyin2phone[85].yunmu,"an");
	strcpy(pinyin2phone[86].pinyin,"fang");
	strcpy(pinyin2phone[86].shengmu,"f");
	strcpy(pinyin2phone[86].yunmu,"ang");
	strcpy(pinyin2phone[87].pinyin,"fei");
	strcpy(pinyin2phone[87].shengmu,"f");
	strcpy(pinyin2phone[87].yunmu,"ei");
	strcpy(pinyin2phone[88].pinyin,"fen");
	strcpy(pinyin2phone[88].shengmu,"f");
	strcpy(pinyin2phone[88].yunmu,"en");
	strcpy(pinyin2phone[89].pinyin,"feng");
	strcpy(pinyin2phone[89].shengmu,"f");
	strcpy(pinyin2phone[89].yunmu,"eng");
	strcpy(pinyin2phone[90].pinyin,"fo");
	strcpy(pinyin2phone[90].shengmu,"f");
	strcpy(pinyin2phone[90].yunmu,"o");
	strcpy(pinyin2phone[91].pinyin,"fou");
	strcpy(pinyin2phone[91].shengmu,"f");
	strcpy(pinyin2phone[91].yunmu,"ou");
	strcpy(pinyin2phone[92].pinyin,"fu");
	strcpy(pinyin2phone[92].shengmu,"f");
	strcpy(pinyin2phone[92].yunmu,"u");
	strcpy(pinyin2phone[93].pinyin,"ga");
	strcpy(pinyin2phone[93].shengmu,"g");
	strcpy(pinyin2phone[93].yunmu,"a");
	strcpy(pinyin2phone[94].pinyin,"gai");
	strcpy(pinyin2phone[94].shengmu,"g");
	strcpy(pinyin2phone[94].yunmu,"ai");
	strcpy(pinyin2phone[95].pinyin,"gan");
	strcpy(pinyin2phone[95].shengmu,"g");
	strcpy(pinyin2phone[95].yunmu,"an");
	strcpy(pinyin2phone[96].pinyin,"gang");
	strcpy(pinyin2phone[96].shengmu,"g");
	strcpy(pinyin2phone[96].yunmu,"ang");
	strcpy(pinyin2phone[97].pinyin,"gao");
	strcpy(pinyin2phone[97].shengmu,"g");
	strcpy(pinyin2phone[97].yunmu,"ao");
	strcpy(pinyin2phone[98].pinyin,"ge");
	strcpy(pinyin2phone[98].shengmu,"g");
	strcpy(pinyin2phone[98].yunmu,"e");
	strcpy(pinyin2phone[99].pinyin,"gei");
	strcpy(pinyin2phone[99].shengmu,"g");
	strcpy(pinyin2phone[99].yunmu,"ei");
	strcpy(pinyin2phone[100].pinyin,"gen");
	strcpy(pinyin2phone[100].shengmu,"g");
	strcpy(pinyin2phone[100].yunmu,"en");
	strcpy(pinyin2phone[101].pinyin,"geng");
	strcpy(pinyin2phone[101].shengmu,"g");
	strcpy(pinyin2phone[101].yunmu,"eng");
	strcpy(pinyin2phone[102].pinyin,"gong");
	strcpy(pinyin2phone[102].shengmu,"g");
	strcpy(pinyin2phone[102].yunmu,"ong");
	strcpy(pinyin2phone[103].pinyin,"gou");
	strcpy(pinyin2phone[103].shengmu,"g");
	strcpy(pinyin2phone[103].yunmu,"ou");
	strcpy(pinyin2phone[104].pinyin,"gu");
	strcpy(pinyin2phone[104].shengmu,"g");
	strcpy(pinyin2phone[104].yunmu,"u");
	strcpy(pinyin2phone[105].pinyin,"gua");
	strcpy(pinyin2phone[105].shengmu,"g");
	strcpy(pinyin2phone[105].yunmu,"ua");
	strcpy(pinyin2phone[106].pinyin,"guai");
	strcpy(pinyin2phone[106].shengmu,"g");
	strcpy(pinyin2phone[106].yunmu,"uai");
	strcpy(pinyin2phone[107].pinyin,"guan");
	strcpy(pinyin2phone[107].shengmu,"g");
	strcpy(pinyin2phone[107].yunmu,"uan");
	strcpy(pinyin2phone[108].pinyin,"guang");
	strcpy(pinyin2phone[108].shengmu,"g");
	strcpy(pinyin2phone[108].yunmu,"uang");
	strcpy(pinyin2phone[109].pinyin,"gui");
	strcpy(pinyin2phone[109].shengmu,"g");
	strcpy(pinyin2phone[109].yunmu,"ui");
	strcpy(pinyin2phone[110].pinyin,"gun");
	strcpy(pinyin2phone[110].shengmu,"g");
	strcpy(pinyin2phone[110].yunmu,"un");
	strcpy(pinyin2phone[111].pinyin,"guo");
	strcpy(pinyin2phone[111].shengmu,"g");
	strcpy(pinyin2phone[111].yunmu,"uo");
	strcpy(pinyin2phone[112].pinyin,"ha");
	strcpy(pinyin2phone[112].shengmu,"h");
	strcpy(pinyin2phone[112].yunmu,"a");
	strcpy(pinyin2phone[113].pinyin,"hai");
	strcpy(pinyin2phone[113].shengmu,"h");
	strcpy(pinyin2phone[113].yunmu,"ai");
	strcpy(pinyin2phone[114].pinyin,"han");
	strcpy(pinyin2phone[114].shengmu,"h");
	strcpy(pinyin2phone[114].yunmu,"an");
	strcpy(pinyin2phone[115].pinyin,"hang");
	strcpy(pinyin2phone[115].shengmu,"h");
	strcpy(pinyin2phone[115].yunmu,"ang");
	strcpy(pinyin2phone[116].pinyin,"hao");
	strcpy(pinyin2phone[116].shengmu,"h");
	strcpy(pinyin2phone[116].yunmu,"ao");
	strcpy(pinyin2phone[117].pinyin,"he");
	strcpy(pinyin2phone[117].shengmu,"h");
	strcpy(pinyin2phone[117].yunmu,"e");
	strcpy(pinyin2phone[118].pinyin,"hei");
	strcpy(pinyin2phone[118].shengmu,"h");
	strcpy(pinyin2phone[118].yunmu,"ei");
	strcpy(pinyin2phone[119].pinyin,"hen");
	strcpy(pinyin2phone[119].shengmu,"h");
	strcpy(pinyin2phone[119].yunmu,"en");
	strcpy(pinyin2phone[120].pinyin,"heng");
	strcpy(pinyin2phone[120].shengmu,"h");
	strcpy(pinyin2phone[120].yunmu,"eng");
	strcpy(pinyin2phone[121].pinyin,"hong");
	strcpy(pinyin2phone[121].shengmu,"h");
	strcpy(pinyin2phone[121].yunmu,"ong");
	strcpy(pinyin2phone[122].pinyin,"hou");
	strcpy(pinyin2phone[122].shengmu,"h");
	strcpy(pinyin2phone[122].yunmu,"ou");
	strcpy(pinyin2phone[123].pinyin,"hu");
	strcpy(pinyin2phone[123].shengmu,"h");
	strcpy(pinyin2phone[123].yunmu,"u");
	strcpy(pinyin2phone[124].pinyin,"hua");
	strcpy(pinyin2phone[124].shengmu,"h");
	strcpy(pinyin2phone[124].yunmu,"ua");
	strcpy(pinyin2phone[125].pinyin,"huai");
	strcpy(pinyin2phone[125].shengmu,"h");
	strcpy(pinyin2phone[125].yunmu,"uai");
	strcpy(pinyin2phone[126].pinyin,"huan");
	strcpy(pinyin2phone[126].shengmu,"h");
	strcpy(pinyin2phone[126].yunmu,"uan");
	strcpy(pinyin2phone[127].pinyin,"huang");
	strcpy(pinyin2phone[127].shengmu,"h");
	strcpy(pinyin2phone[127].yunmu,"uang");
	strcpy(pinyin2phone[128].pinyin,"hui");
	strcpy(pinyin2phone[128].shengmu,"h");
	strcpy(pinyin2phone[128].yunmu,"ui");
	strcpy(pinyin2phone[129].pinyin,"hun");
	strcpy(pinyin2phone[129].shengmu,"h");
	strcpy(pinyin2phone[129].yunmu,"un");
	strcpy(pinyin2phone[130].pinyin,"huo");
	strcpy(pinyin2phone[130].shengmu,"h");
	strcpy(pinyin2phone[130].yunmu,"uo");
	strcpy(pinyin2phone[131].pinyin,"ji");
	strcpy(pinyin2phone[131].shengmu,"j");
	strcpy(pinyin2phone[131].yunmu,"i");
	strcpy(pinyin2phone[132].pinyin,"jia");
	strcpy(pinyin2phone[132].shengmu,"j");
	strcpy(pinyin2phone[132].yunmu,"ia");
	strcpy(pinyin2phone[133].pinyin,"jian");
	strcpy(pinyin2phone[133].shengmu,"j");
	strcpy(pinyin2phone[133].yunmu,"ian");
	strcpy(pinyin2phone[134].pinyin,"jiang");
	strcpy(pinyin2phone[134].shengmu,"j");
	strcpy(pinyin2phone[134].yunmu,"iang");
	strcpy(pinyin2phone[135].pinyin,"jiao");
	strcpy(pinyin2phone[135].shengmu,"j");
	strcpy(pinyin2phone[135].yunmu,"iao");
	strcpy(pinyin2phone[136].pinyin,"jie");
	strcpy(pinyin2phone[136].shengmu,"j");
	strcpy(pinyin2phone[136].yunmu,"ie");
	strcpy(pinyin2phone[137].pinyin,"jin");
	strcpy(pinyin2phone[137].shengmu,"j");
	strcpy(pinyin2phone[137].yunmu,"in");
	strcpy(pinyin2phone[138].pinyin,"jing");
	strcpy(pinyin2phone[138].shengmu,"j");
	strcpy(pinyin2phone[138].yunmu,"ing");
	strcpy(pinyin2phone[139].pinyin,"jiong");
	strcpy(pinyin2phone[139].shengmu,"j");
	strcpy(pinyin2phone[139].yunmu,"iong");
	strcpy(pinyin2phone[140].pinyin,"jiu");
	strcpy(pinyin2phone[140].shengmu,"j");
	strcpy(pinyin2phone[140].yunmu,"iu");
	strcpy(pinyin2phone[141].pinyin,"ju");
	strcpy(pinyin2phone[141].shengmu,"j");
	strcpy(pinyin2phone[141].yunmu,"ux");
	strcpy(pinyin2phone[142].pinyin,"juan");
	strcpy(pinyin2phone[142].shengmu,"j");
	strcpy(pinyin2phone[142].yunmu,"uxan");
	strcpy(pinyin2phone[143].pinyin,"jue");
	strcpy(pinyin2phone[143].shengmu,"j");
	strcpy(pinyin2phone[143].yunmu,"uxe");
	strcpy(pinyin2phone[144].pinyin,"jun");
	strcpy(pinyin2phone[144].shengmu,"j");
	strcpy(pinyin2phone[144].yunmu,"uxn");
	strcpy(pinyin2phone[145].pinyin,"ka");
	strcpy(pinyin2phone[145].shengmu,"k");
	strcpy(pinyin2phone[145].yunmu,"a");
	strcpy(pinyin2phone[146].pinyin,"kai");
	strcpy(pinyin2phone[146].shengmu,"k");
	strcpy(pinyin2phone[146].yunmu,"ai");
	strcpy(pinyin2phone[147].pinyin,"kan");
	strcpy(pinyin2phone[147].shengmu,"k");
	strcpy(pinyin2phone[147].yunmu,"an");
	strcpy(pinyin2phone[148].pinyin,"kang");
	strcpy(pinyin2phone[148].shengmu,"k");
	strcpy(pinyin2phone[148].yunmu,"ang");
	strcpy(pinyin2phone[149].pinyin,"kao");
	strcpy(pinyin2phone[149].shengmu,"k");
	strcpy(pinyin2phone[149].yunmu,"ao");
	strcpy(pinyin2phone[150].pinyin,"ke");
	strcpy(pinyin2phone[150].shengmu,"k");
	strcpy(pinyin2phone[150].yunmu,"e");
	strcpy(pinyin2phone[151].pinyin,"kei");
	strcpy(pinyin2phone[151].shengmu,"k");
	strcpy(pinyin2phone[151].yunmu,"ei");
	strcpy(pinyin2phone[152].pinyin,"ken");
	strcpy(pinyin2phone[152].shengmu,"k");
	strcpy(pinyin2phone[152].yunmu,"en");
	strcpy(pinyin2phone[153].pinyin,"keng");
	strcpy(pinyin2phone[153].shengmu,"k");
	strcpy(pinyin2phone[153].yunmu,"eng");
	strcpy(pinyin2phone[154].pinyin,"kong");
	strcpy(pinyin2phone[154].shengmu,"k");
	strcpy(pinyin2phone[154].yunmu,"ong");
	strcpy(pinyin2phone[155].pinyin,"kou");
	strcpy(pinyin2phone[155].shengmu,"k");
	strcpy(pinyin2phone[155].yunmu,"ou");
	strcpy(pinyin2phone[156].pinyin,"ku");
	strcpy(pinyin2phone[156].shengmu,"k");
	strcpy(pinyin2phone[156].yunmu,"u");
	strcpy(pinyin2phone[157].pinyin,"kua");
	strcpy(pinyin2phone[157].shengmu,"k");
	strcpy(pinyin2phone[157].yunmu,"ua");
	strcpy(pinyin2phone[158].pinyin,"kuai");
	strcpy(pinyin2phone[158].shengmu,"k");
	strcpy(pinyin2phone[158].yunmu,"uai");
	strcpy(pinyin2phone[159].pinyin,"kuan");
	strcpy(pinyin2phone[159].shengmu,"k");
	strcpy(pinyin2phone[159].yunmu,"uan");
	strcpy(pinyin2phone[160].pinyin,"kuang");
	strcpy(pinyin2phone[160].shengmu,"k");
	strcpy(pinyin2phone[160].yunmu,"uang");
	strcpy(pinyin2phone[161].pinyin,"kui");
	strcpy(pinyin2phone[161].shengmu,"k");
	strcpy(pinyin2phone[161].yunmu,"ui");
	strcpy(pinyin2phone[162].pinyin,"kun");
	strcpy(pinyin2phone[162].shengmu,"k");
	strcpy(pinyin2phone[162].yunmu,"un");
	strcpy(pinyin2phone[163].pinyin,"kuo");
	strcpy(pinyin2phone[163].shengmu,"k");
	strcpy(pinyin2phone[163].yunmu,"uo");
	strcpy(pinyin2phone[164].pinyin,"la");
	strcpy(pinyin2phone[164].shengmu,"l");
	strcpy(pinyin2phone[164].yunmu,"a");
	strcpy(pinyin2phone[165].pinyin,"lai");
	strcpy(pinyin2phone[165].shengmu,"l");
	strcpy(pinyin2phone[165].yunmu,"ai");
	strcpy(pinyin2phone[166].pinyin,"lan");
	strcpy(pinyin2phone[166].shengmu,"l");
	strcpy(pinyin2phone[166].yunmu,"an");
	strcpy(pinyin2phone[167].pinyin,"lang");
	strcpy(pinyin2phone[167].shengmu,"l");
	strcpy(pinyin2phone[167].yunmu,"ang");
	strcpy(pinyin2phone[168].pinyin,"lao");
	strcpy(pinyin2phone[168].shengmu,"l");
	strcpy(pinyin2phone[168].yunmu,"ao");
	strcpy(pinyin2phone[169].pinyin,"le");
	strcpy(pinyin2phone[169].shengmu,"l");
	strcpy(pinyin2phone[169].yunmu,"e");
	strcpy(pinyin2phone[170].pinyin,"lei");
	strcpy(pinyin2phone[170].shengmu,"l");
	strcpy(pinyin2phone[170].yunmu,"ei");
	strcpy(pinyin2phone[171].pinyin,"leng");
	strcpy(pinyin2phone[171].shengmu,"l");
	strcpy(pinyin2phone[171].yunmu,"eng");
	strcpy(pinyin2phone[172].pinyin,"li");
	strcpy(pinyin2phone[172].shengmu,"l");
	strcpy(pinyin2phone[172].yunmu,"i");
	strcpy(pinyin2phone[173].pinyin,"lia");
	strcpy(pinyin2phone[173].shengmu,"l");
	strcpy(pinyin2phone[173].yunmu,"ia");
	strcpy(pinyin2phone[174].pinyin,"lian");
	strcpy(pinyin2phone[174].shengmu,"l");
	strcpy(pinyin2phone[174].yunmu,"ian");
	strcpy(pinyin2phone[175].pinyin,"liang");
	strcpy(pinyin2phone[175].shengmu,"l");
	strcpy(pinyin2phone[175].yunmu,"iang");
	strcpy(pinyin2phone[176].pinyin,"liao");
	strcpy(pinyin2phone[176].shengmu,"l");
	strcpy(pinyin2phone[176].yunmu,"iao");
	strcpy(pinyin2phone[177].pinyin,"lie");
	strcpy(pinyin2phone[177].shengmu,"l");
	strcpy(pinyin2phone[177].yunmu,"ie");
	strcpy(pinyin2phone[178].pinyin,"lin");
	strcpy(pinyin2phone[178].shengmu,"l");
	strcpy(pinyin2phone[178].yunmu,"in");
	strcpy(pinyin2phone[179].pinyin,"ling");
	strcpy(pinyin2phone[179].shengmu,"l");
	strcpy(pinyin2phone[179].yunmu,"ing");
	strcpy(pinyin2phone[180].pinyin,"liu");
	strcpy(pinyin2phone[180].shengmu,"l");
	strcpy(pinyin2phone[180].yunmu,"iu");
	strcpy(pinyin2phone[181].pinyin,"lo");
	strcpy(pinyin2phone[181].shengmu,"l");
	strcpy(pinyin2phone[181].yunmu,"o");
	strcpy(pinyin2phone[182].pinyin,"long");
	strcpy(pinyin2phone[182].shengmu,"l");
	strcpy(pinyin2phone[182].yunmu,"ong");
	strcpy(pinyin2phone[183].pinyin,"lou");
	strcpy(pinyin2phone[183].shengmu,"l");
	strcpy(pinyin2phone[183].yunmu,"ou");
	strcpy(pinyin2phone[184].pinyin,"lu");
	strcpy(pinyin2phone[184].shengmu,"l");
	strcpy(pinyin2phone[184].yunmu,"u");
	strcpy(pinyin2phone[185].pinyin,"luan");
	strcpy(pinyin2phone[185].shengmu,"l");
	strcpy(pinyin2phone[185].yunmu,"uan");
	strcpy(pinyin2phone[186].pinyin,"lun");
	strcpy(pinyin2phone[186].shengmu,"l");
	strcpy(pinyin2phone[186].yunmu,"un");
	strcpy(pinyin2phone[187].pinyin,"luo");
	strcpy(pinyin2phone[187].shengmu,"l");
	strcpy(pinyin2phone[187].yunmu,"uo");
	strcpy(pinyin2phone[188].pinyin,"lv");
	strcpy(pinyin2phone[188].shengmu,"l");
	strcpy(pinyin2phone[188].yunmu,"ux");
	strcpy(pinyin2phone[189].pinyin,"lve");
	strcpy(pinyin2phone[189].shengmu,"l");
	strcpy(pinyin2phone[189].yunmu,"uxe");
	strcpy(pinyin2phone[190].pinyin,"ma");
	strcpy(pinyin2phone[190].shengmu,"m");
	strcpy(pinyin2phone[190].yunmu,"a");
	strcpy(pinyin2phone[191].pinyin,"mai");
	strcpy(pinyin2phone[191].shengmu,"m");
	strcpy(pinyin2phone[191].yunmu,"ai");
	strcpy(pinyin2phone[192].pinyin,"man");
	strcpy(pinyin2phone[192].shengmu,"m");
	strcpy(pinyin2phone[192].yunmu,"an");
	strcpy(pinyin2phone[193].pinyin,"mang");
	strcpy(pinyin2phone[193].shengmu,"m");
	strcpy(pinyin2phone[193].yunmu,"ang");
	strcpy(pinyin2phone[194].pinyin,"mao");
	strcpy(pinyin2phone[194].shengmu,"m");
	strcpy(pinyin2phone[194].yunmu,"ao");
	strcpy(pinyin2phone[195].pinyin,"me");
	strcpy(pinyin2phone[195].shengmu,"m");
	strcpy(pinyin2phone[195].yunmu,"e");
	strcpy(pinyin2phone[196].pinyin,"mei");
	strcpy(pinyin2phone[196].shengmu,"m");
	strcpy(pinyin2phone[196].yunmu,"ei");
	strcpy(pinyin2phone[197].pinyin,"men");
	strcpy(pinyin2phone[197].shengmu,"m");
	strcpy(pinyin2phone[197].yunmu,"en");
	strcpy(pinyin2phone[198].pinyin,"meng");
	strcpy(pinyin2phone[198].shengmu,"m");
	strcpy(pinyin2phone[198].yunmu,"eng");
	strcpy(pinyin2phone[199].pinyin,"mi");
	strcpy(pinyin2phone[199].shengmu,"m");
	strcpy(pinyin2phone[199].yunmu,"i");
	strcpy(pinyin2phone[200].pinyin,"mian");
	strcpy(pinyin2phone[200].shengmu,"m");
	strcpy(pinyin2phone[200].yunmu,"ian");
	strcpy(pinyin2phone[201].pinyin,"miao");
	strcpy(pinyin2phone[201].shengmu,"m");
	strcpy(pinyin2phone[201].yunmu,"iao");
	strcpy(pinyin2phone[202].pinyin,"mie");
	strcpy(pinyin2phone[202].shengmu,"m");
	strcpy(pinyin2phone[202].yunmu,"ie");
	strcpy(pinyin2phone[203].pinyin,"min");
	strcpy(pinyin2phone[203].shengmu,"m");
	strcpy(pinyin2phone[203].yunmu,"in");
	strcpy(pinyin2phone[204].pinyin,"ming");
	strcpy(pinyin2phone[204].shengmu,"m");
	strcpy(pinyin2phone[204].yunmu,"ing");
	strcpy(pinyin2phone[205].pinyin,"miu");
	strcpy(pinyin2phone[205].shengmu,"m");
	strcpy(pinyin2phone[205].yunmu,"iu");
	strcpy(pinyin2phone[206].pinyin,"mo");
	strcpy(pinyin2phone[206].shengmu,"m");
	strcpy(pinyin2phone[206].yunmu,"o");
	strcpy(pinyin2phone[207].pinyin,"mou");
	strcpy(pinyin2phone[207].shengmu,"m");
	strcpy(pinyin2phone[207].yunmu,"ou");
	strcpy(pinyin2phone[208].pinyin,"mu");
	strcpy(pinyin2phone[208].shengmu,"m");
	strcpy(pinyin2phone[208].yunmu,"u");
	strcpy(pinyin2phone[209].pinyin,"na");
	strcpy(pinyin2phone[209].shengmu,"n");
	strcpy(pinyin2phone[209].yunmu,"a");
	strcpy(pinyin2phone[210].pinyin,"nai");
	strcpy(pinyin2phone[210].shengmu,"n");
	strcpy(pinyin2phone[210].yunmu,"ai");
	strcpy(pinyin2phone[211].pinyin,"nan");
	strcpy(pinyin2phone[211].shengmu,"n");
	strcpy(pinyin2phone[211].yunmu,"an");
	strcpy(pinyin2phone[212].pinyin,"nang");
	strcpy(pinyin2phone[212].shengmu,"n");
	strcpy(pinyin2phone[212].yunmu,"ang");
	strcpy(pinyin2phone[213].pinyin,"nao");
	strcpy(pinyin2phone[213].shengmu,"n");
	strcpy(pinyin2phone[213].yunmu,"ao");
	strcpy(pinyin2phone[214].pinyin,"ne");
	strcpy(pinyin2phone[214].shengmu,"n");
	strcpy(pinyin2phone[214].yunmu,"e");
	strcpy(pinyin2phone[215].pinyin,"nei");
	strcpy(pinyin2phone[215].shengmu,"n");
	strcpy(pinyin2phone[215].yunmu,"ei");
	strcpy(pinyin2phone[216].pinyin,"nen");
	strcpy(pinyin2phone[216].shengmu,"n");
	strcpy(pinyin2phone[216].yunmu,"en");
	strcpy(pinyin2phone[217].pinyin,"neng");
	strcpy(pinyin2phone[217].shengmu,"n");
	strcpy(pinyin2phone[217].yunmu,"eng");
	strcpy(pinyin2phone[218].pinyin,"ni");
	strcpy(pinyin2phone[218].shengmu,"n");
	strcpy(pinyin2phone[218].yunmu,"i");
	strcpy(pinyin2phone[219].pinyin,"nia");
	strcpy(pinyin2phone[219].shengmu,"n");
	strcpy(pinyin2phone[219].yunmu,"ia");
	strcpy(pinyin2phone[220].pinyin,"nian");
	strcpy(pinyin2phone[220].shengmu,"n");
	strcpy(pinyin2phone[220].yunmu,"ian");
	strcpy(pinyin2phone[221].pinyin,"niang");
	strcpy(pinyin2phone[221].shengmu,"n");
	strcpy(pinyin2phone[221].yunmu,"iang");
	strcpy(pinyin2phone[222].pinyin,"niao");
	strcpy(pinyin2phone[222].shengmu,"n");
	strcpy(pinyin2phone[222].yunmu,"iao");
	strcpy(pinyin2phone[223].pinyin,"nie");
	strcpy(pinyin2phone[223].shengmu,"n");
	strcpy(pinyin2phone[223].yunmu,"ie");
	strcpy(pinyin2phone[224].pinyin,"nin");
	strcpy(pinyin2phone[224].shengmu,"n");
	strcpy(pinyin2phone[224].yunmu,"in");
	strcpy(pinyin2phone[225].pinyin,"ning");
	strcpy(pinyin2phone[225].shengmu,"n");
	strcpy(pinyin2phone[225].yunmu,"ing");
	strcpy(pinyin2phone[226].pinyin,"niu");
	strcpy(pinyin2phone[226].shengmu,"n");
	strcpy(pinyin2phone[226].yunmu,"iu");
	strcpy(pinyin2phone[227].pinyin,"nong");
	strcpy(pinyin2phone[227].shengmu,"n");
	strcpy(pinyin2phone[227].yunmu,"ong");
	strcpy(pinyin2phone[228].pinyin,"nou");
	strcpy(pinyin2phone[228].shengmu,"n");
	strcpy(pinyin2phone[228].yunmu,"ou");
	strcpy(pinyin2phone[229].pinyin,"nu");
	strcpy(pinyin2phone[229].shengmu,"n");
	strcpy(pinyin2phone[229].yunmu,"u");
	strcpy(pinyin2phone[230].pinyin,"nuan");
	strcpy(pinyin2phone[230].shengmu,"n");
	strcpy(pinyin2phone[230].yunmu,"uan");
	strcpy(pinyin2phone[231].pinyin,"nuo");
	strcpy(pinyin2phone[231].shengmu,"n");
	strcpy(pinyin2phone[231].yunmu,"uo");
	strcpy(pinyin2phone[232].pinyin,"nv");
	strcpy(pinyin2phone[232].shengmu,"n");
	strcpy(pinyin2phone[232].yunmu,"ux");
	strcpy(pinyin2phone[233].pinyin,"nve");
	strcpy(pinyin2phone[233].shengmu,"n");
	strcpy(pinyin2phone[233].yunmu,"uxe");
	strcpy(pinyin2phone[234].pinyin,"o");
	strcpy(pinyin2phone[234].shengmu,"os");
	strcpy(pinyin2phone[234].yunmu,"o");
	strcpy(pinyin2phone[235].pinyin,"ou");
	strcpy(pinyin2phone[235].shengmu,"os");
	strcpy(pinyin2phone[235].yunmu,"ou");
	strcpy(pinyin2phone[236].pinyin,"pa");
	strcpy(pinyin2phone[236].shengmu,"p");
	strcpy(pinyin2phone[236].yunmu,"a");
	strcpy(pinyin2phone[237].pinyin,"pai");
	strcpy(pinyin2phone[237].shengmu,"p");
	strcpy(pinyin2phone[237].yunmu,"ai");
	strcpy(pinyin2phone[238].pinyin,"pan");
	strcpy(pinyin2phone[238].shengmu,"p");
	strcpy(pinyin2phone[238].yunmu,"an");
	strcpy(pinyin2phone[239].pinyin,"pang");
	strcpy(pinyin2phone[239].shengmu,"p");
	strcpy(pinyin2phone[239].yunmu,"ang");
	strcpy(pinyin2phone[240].pinyin,"pao");
	strcpy(pinyin2phone[240].shengmu,"p");
	strcpy(pinyin2phone[240].yunmu,"ao");
	strcpy(pinyin2phone[241].pinyin,"pei");
	strcpy(pinyin2phone[241].shengmu,"p");
	strcpy(pinyin2phone[241].yunmu,"ei");
	strcpy(pinyin2phone[242].pinyin,"pen");
	strcpy(pinyin2phone[242].shengmu,"p");
	strcpy(pinyin2phone[242].yunmu,"en");
	strcpy(pinyin2phone[243].pinyin,"peng");
	strcpy(pinyin2phone[243].shengmu,"p");
	strcpy(pinyin2phone[243].yunmu,"eng");
	strcpy(pinyin2phone[244].pinyin,"pi");
	strcpy(pinyin2phone[244].shengmu,"p");
	strcpy(pinyin2phone[244].yunmu,"i");
	strcpy(pinyin2phone[245].pinyin,"pian");
	strcpy(pinyin2phone[245].shengmu,"p");
	strcpy(pinyin2phone[245].yunmu,"ian");
	strcpy(pinyin2phone[246].pinyin,"piao");
	strcpy(pinyin2phone[246].shengmu,"p");
	strcpy(pinyin2phone[246].yunmu,"iao");
	strcpy(pinyin2phone[247].pinyin,"pie");
	strcpy(pinyin2phone[247].shengmu,"p");
	strcpy(pinyin2phone[247].yunmu,"ie");
	strcpy(pinyin2phone[248].pinyin,"pin");
	strcpy(pinyin2phone[248].shengmu,"p");
	strcpy(pinyin2phone[248].yunmu,"in");
	strcpy(pinyin2phone[249].pinyin,"ping");
	strcpy(pinyin2phone[249].shengmu,"p");
	strcpy(pinyin2phone[249].yunmu,"ing");
	strcpy(pinyin2phone[250].pinyin,"po");
	strcpy(pinyin2phone[250].shengmu,"p");
	strcpy(pinyin2phone[250].yunmu,"o");
	strcpy(pinyin2phone[251].pinyin,"pou");
	strcpy(pinyin2phone[251].shengmu,"p");
	strcpy(pinyin2phone[251].yunmu,"ou");
	strcpy(pinyin2phone[252].pinyin,"pu");
	strcpy(pinyin2phone[252].shengmu,"p");
	strcpy(pinyin2phone[252].yunmu,"u");
	strcpy(pinyin2phone[253].pinyin,"qi");
	strcpy(pinyin2phone[253].shengmu,"q");
	strcpy(pinyin2phone[253].yunmu,"i");
	strcpy(pinyin2phone[254].pinyin,"qia");
	strcpy(pinyin2phone[254].shengmu,"q");
	strcpy(pinyin2phone[254].yunmu,"ia");
	strcpy(pinyin2phone[255].pinyin,"qian");
	strcpy(pinyin2phone[255].shengmu,"q");
	strcpy(pinyin2phone[255].yunmu,"ian");
	strcpy(pinyin2phone[256].pinyin,"qiang");
	strcpy(pinyin2phone[256].shengmu,"q");
	strcpy(pinyin2phone[256].yunmu,"iang");
	strcpy(pinyin2phone[257].pinyin,"qiao");
	strcpy(pinyin2phone[257].shengmu,"q");
	strcpy(pinyin2phone[257].yunmu,"iao");
	strcpy(pinyin2phone[258].pinyin,"qie");
	strcpy(pinyin2phone[258].shengmu,"q");
	strcpy(pinyin2phone[258].yunmu,"ie");
	strcpy(pinyin2phone[259].pinyin,"qin");
	strcpy(pinyin2phone[259].shengmu,"q");
	strcpy(pinyin2phone[259].yunmu,"in");
	strcpy(pinyin2phone[260].pinyin,"qing");
	strcpy(pinyin2phone[260].shengmu,"q");
	strcpy(pinyin2phone[260].yunmu,"ing");
	strcpy(pinyin2phone[261].pinyin,"qiong");
	strcpy(pinyin2phone[261].shengmu,"q");
	strcpy(pinyin2phone[261].yunmu,"iong");
	strcpy(pinyin2phone[262].pinyin,"qiu");
	strcpy(pinyin2phone[262].shengmu,"q");
	strcpy(pinyin2phone[262].yunmu,"iu");
	strcpy(pinyin2phone[263].pinyin,"qu");
	strcpy(pinyin2phone[263].shengmu,"q");
	strcpy(pinyin2phone[263].yunmu,"ux");
	strcpy(pinyin2phone[264].pinyin,"quan");
	strcpy(pinyin2phone[264].shengmu,"q");
	strcpy(pinyin2phone[264].yunmu,"uxan");
	strcpy(pinyin2phone[265].pinyin,"que");
	strcpy(pinyin2phone[265].shengmu,"q");
	strcpy(pinyin2phone[265].yunmu,"uxe");
	strcpy(pinyin2phone[266].pinyin,"qun");
	strcpy(pinyin2phone[266].shengmu,"q");
	strcpy(pinyin2phone[266].yunmu,"uxn");
	strcpy(pinyin2phone[267].pinyin,"ran");
	strcpy(pinyin2phone[267].shengmu,"r");
	strcpy(pinyin2phone[267].yunmu,"an");
	strcpy(pinyin2phone[268].pinyin,"rang");
	strcpy(pinyin2phone[268].shengmu,"r");
	strcpy(pinyin2phone[268].yunmu,"ang");
	strcpy(pinyin2phone[269].pinyin,"rao");
	strcpy(pinyin2phone[269].shengmu,"r");
	strcpy(pinyin2phone[269].yunmu,"ao");
	strcpy(pinyin2phone[270].pinyin,"re");
	strcpy(pinyin2phone[270].shengmu,"r");
	strcpy(pinyin2phone[270].yunmu,"e");
	strcpy(pinyin2phone[271].pinyin,"ren");
	strcpy(pinyin2phone[271].shengmu,"r");
	strcpy(pinyin2phone[271].yunmu,"en");
	strcpy(pinyin2phone[272].pinyin,"reng");
	strcpy(pinyin2phone[272].shengmu,"r");
	strcpy(pinyin2phone[272].yunmu,"eng");
	strcpy(pinyin2phone[273].pinyin,"ri");
	strcpy(pinyin2phone[273].shengmu,"r");
	strcpy(pinyin2phone[273].yunmu,"ib");
	strcpy(pinyin2phone[274].pinyin,"rong");
	strcpy(pinyin2phone[274].shengmu,"r");
	strcpy(pinyin2phone[274].yunmu,"ong");
	strcpy(pinyin2phone[275].pinyin,"rou");
	strcpy(pinyin2phone[275].shengmu,"r");
	strcpy(pinyin2phone[275].yunmu,"ou");
	strcpy(pinyin2phone[276].pinyin,"ru");
	strcpy(pinyin2phone[276].shengmu,"r");
	strcpy(pinyin2phone[276].yunmu,"u");
	strcpy(pinyin2phone[277].pinyin,"rua");
	strcpy(pinyin2phone[277].shengmu,"r");
	strcpy(pinyin2phone[277].yunmu,"ua");
	strcpy(pinyin2phone[278].pinyin,"ruan");
	strcpy(pinyin2phone[278].shengmu,"r");
	strcpy(pinyin2phone[278].yunmu,"uan");
	strcpy(pinyin2phone[279].pinyin,"rui");
	strcpy(pinyin2phone[279].shengmu,"r");
	strcpy(pinyin2phone[279].yunmu,"ui");
	strcpy(pinyin2phone[280].pinyin,"run");
	strcpy(pinyin2phone[280].shengmu,"r");
	strcpy(pinyin2phone[280].yunmu,"un");
	strcpy(pinyin2phone[281].pinyin,"ruo");
	strcpy(pinyin2phone[281].shengmu,"r");
	strcpy(pinyin2phone[281].yunmu,"uo");
	strcpy(pinyin2phone[282].pinyin,"sa");
	strcpy(pinyin2phone[282].shengmu,"s");
	strcpy(pinyin2phone[282].yunmu,"a");
	strcpy(pinyin2phone[283].pinyin,"sai");
	strcpy(pinyin2phone[283].shengmu,"s");
	strcpy(pinyin2phone[283].yunmu,"ai");
	strcpy(pinyin2phone[284].pinyin,"san");
	strcpy(pinyin2phone[284].shengmu,"s");
	strcpy(pinyin2phone[284].yunmu,"an");
	strcpy(pinyin2phone[285].pinyin,"sang");
	strcpy(pinyin2phone[285].shengmu,"s");
	strcpy(pinyin2phone[285].yunmu,"ang");
	strcpy(pinyin2phone[286].pinyin,"sao");
	strcpy(pinyin2phone[286].shengmu,"s");
	strcpy(pinyin2phone[286].yunmu,"ao");
	strcpy(pinyin2phone[287].pinyin,"se");
	strcpy(pinyin2phone[287].shengmu,"s");
	strcpy(pinyin2phone[287].yunmu,"e");
	strcpy(pinyin2phone[288].pinyin,"sen");
	strcpy(pinyin2phone[288].shengmu,"s");
	strcpy(pinyin2phone[288].yunmu,"en");
	strcpy(pinyin2phone[289].pinyin,"seng");
	strcpy(pinyin2phone[289].shengmu,"s");
	strcpy(pinyin2phone[289].yunmu,"eng");
	strcpy(pinyin2phone[290].pinyin,"sha");
	strcpy(pinyin2phone[290].shengmu,"sh");
	strcpy(pinyin2phone[290].yunmu,"a");
	strcpy(pinyin2phone[291].pinyin,"shai");
	strcpy(pinyin2phone[291].shengmu,"sh");
	strcpy(pinyin2phone[291].yunmu,"ai");
	strcpy(pinyin2phone[292].pinyin,"shan");
	strcpy(pinyin2phone[292].shengmu,"sh");
	strcpy(pinyin2phone[292].yunmu,"an");
	strcpy(pinyin2phone[293].pinyin,"shang");
	strcpy(pinyin2phone[293].shengmu,"sh");
	strcpy(pinyin2phone[293].yunmu,"ang");
	strcpy(pinyin2phone[294].pinyin,"shao");
	strcpy(pinyin2phone[294].shengmu,"sh");
	strcpy(pinyin2phone[294].yunmu,"ao");
	strcpy(pinyin2phone[295].pinyin,"she");
	strcpy(pinyin2phone[295].shengmu,"sh");
	strcpy(pinyin2phone[295].yunmu,"e");
	strcpy(pinyin2phone[296].pinyin,"shei");
	strcpy(pinyin2phone[296].shengmu,"sh");
	strcpy(pinyin2phone[296].yunmu,"ei");
	strcpy(pinyin2phone[297].pinyin,"shen");
	strcpy(pinyin2phone[297].shengmu,"sh");
	strcpy(pinyin2phone[297].yunmu,"en");
	strcpy(pinyin2phone[298].pinyin,"sheng");
	strcpy(pinyin2phone[298].shengmu,"sh");
	strcpy(pinyin2phone[298].yunmu,"eng");
	strcpy(pinyin2phone[299].pinyin,"shi");
	strcpy(pinyin2phone[299].shengmu,"sh");
	strcpy(pinyin2phone[299].yunmu,"ib");
	strcpy(pinyin2phone[300].pinyin,"shou");
	strcpy(pinyin2phone[300].shengmu,"sh");
	strcpy(pinyin2phone[300].yunmu,"ou");
	strcpy(pinyin2phone[301].pinyin,"shu");
	strcpy(pinyin2phone[301].shengmu,"sh");
	strcpy(pinyin2phone[301].yunmu,"u");
	strcpy(pinyin2phone[302].pinyin,"shua");
	strcpy(pinyin2phone[302].shengmu,"sh");
	strcpy(pinyin2phone[302].yunmu,"ua");
	strcpy(pinyin2phone[303].pinyin,"shuai");
	strcpy(pinyin2phone[303].shengmu,"sh");
	strcpy(pinyin2phone[303].yunmu,"uai");
	strcpy(pinyin2phone[304].pinyin,"shuan");
	strcpy(pinyin2phone[304].shengmu,"sh");
	strcpy(pinyin2phone[304].yunmu,"uan");
	strcpy(pinyin2phone[305].pinyin,"shuang");
	strcpy(pinyin2phone[305].shengmu,"sh");
	strcpy(pinyin2phone[305].yunmu,"uang");
	strcpy(pinyin2phone[306].pinyin,"shui");
	strcpy(pinyin2phone[306].shengmu,"sh");
	strcpy(pinyin2phone[306].yunmu,"ui");
	strcpy(pinyin2phone[307].pinyin,"shun");
	strcpy(pinyin2phone[307].shengmu,"sh");
	strcpy(pinyin2phone[307].yunmu,"un");
	strcpy(pinyin2phone[308].pinyin,"shuo");
	strcpy(pinyin2phone[308].shengmu,"sh");
	strcpy(pinyin2phone[308].yunmu,"uo");
	strcpy(pinyin2phone[309].pinyin,"si");
	strcpy(pinyin2phone[309].shengmu,"s");
	strcpy(pinyin2phone[309].yunmu,"if");
	strcpy(pinyin2phone[310].pinyin,"song");
	strcpy(pinyin2phone[310].shengmu,"s");
	strcpy(pinyin2phone[310].yunmu,"ong");
	strcpy(pinyin2phone[311].pinyin,"sou");
	strcpy(pinyin2phone[311].shengmu,"s");
	strcpy(pinyin2phone[311].yunmu,"ou");
	strcpy(pinyin2phone[312].pinyin,"su");
	strcpy(pinyin2phone[312].shengmu,"s");
	strcpy(pinyin2phone[312].yunmu,"u");
	strcpy(pinyin2phone[313].pinyin,"suan");
	strcpy(pinyin2phone[313].shengmu,"s");
	strcpy(pinyin2phone[313].yunmu,"uan");
	strcpy(pinyin2phone[314].pinyin,"sui");
	strcpy(pinyin2phone[314].shengmu,"s");
	strcpy(pinyin2phone[314].yunmu,"ui");
	strcpy(pinyin2phone[315].pinyin,"sun");
	strcpy(pinyin2phone[315].shengmu,"s");
	strcpy(pinyin2phone[315].yunmu,"un");
	strcpy(pinyin2phone[316].pinyin,"suo");
	strcpy(pinyin2phone[316].shengmu,"s");
	strcpy(pinyin2phone[316].yunmu,"uo");
	strcpy(pinyin2phone[317].pinyin,"ta");
	strcpy(pinyin2phone[317].shengmu,"t");
	strcpy(pinyin2phone[317].yunmu,"a");
	strcpy(pinyin2phone[318].pinyin,"tai");
	strcpy(pinyin2phone[318].shengmu,"t");
	strcpy(pinyin2phone[318].yunmu,"ai");
	strcpy(pinyin2phone[319].pinyin,"tan");
	strcpy(pinyin2phone[319].shengmu,"t");
	strcpy(pinyin2phone[319].yunmu,"an");
	strcpy(pinyin2phone[320].pinyin,"tang");
	strcpy(pinyin2phone[320].shengmu,"t");
	strcpy(pinyin2phone[320].yunmu,"ang");
	strcpy(pinyin2phone[321].pinyin,"tao");
	strcpy(pinyin2phone[321].shengmu,"t");
	strcpy(pinyin2phone[321].yunmu,"ao");
	strcpy(pinyin2phone[322].pinyin,"te");
	strcpy(pinyin2phone[322].shengmu,"t");
	strcpy(pinyin2phone[322].yunmu,"e");
	strcpy(pinyin2phone[323].pinyin,"teng");
	strcpy(pinyin2phone[323].shengmu,"t");
	strcpy(pinyin2phone[323].yunmu,"eng");
	strcpy(pinyin2phone[324].pinyin,"ti");
	strcpy(pinyin2phone[324].shengmu,"t");
	strcpy(pinyin2phone[324].yunmu,"i");
	strcpy(pinyin2phone[325].pinyin,"tian");
	strcpy(pinyin2phone[325].shengmu,"t");
	strcpy(pinyin2phone[325].yunmu,"ian");
	strcpy(pinyin2phone[326].pinyin,"tiao");
	strcpy(pinyin2phone[326].shengmu,"t");
	strcpy(pinyin2phone[326].yunmu,"iao");
	strcpy(pinyin2phone[327].pinyin,"tie");
	strcpy(pinyin2phone[327].shengmu,"t");
	strcpy(pinyin2phone[327].yunmu,"ie");
	strcpy(pinyin2phone[328].pinyin,"ting");
	strcpy(pinyin2phone[328].shengmu,"t");
	strcpy(pinyin2phone[328].yunmu,"ing");
	strcpy(pinyin2phone[329].pinyin,"tong");
	strcpy(pinyin2phone[329].shengmu,"t");
	strcpy(pinyin2phone[329].yunmu,"ong");
	strcpy(pinyin2phone[330].pinyin,"tou");
	strcpy(pinyin2phone[330].shengmu,"t");
	strcpy(pinyin2phone[330].yunmu,"ou");
	strcpy(pinyin2phone[331].pinyin,"tu");
	strcpy(pinyin2phone[331].shengmu,"t");
	strcpy(pinyin2phone[331].yunmu,"u");
	strcpy(pinyin2phone[332].pinyin,"tuan");
	strcpy(pinyin2phone[332].shengmu,"t");
	strcpy(pinyin2phone[332].yunmu,"uan");
	strcpy(pinyin2phone[333].pinyin,"tui");
	strcpy(pinyin2phone[333].shengmu,"t");
	strcpy(pinyin2phone[333].yunmu,"ui");
	strcpy(pinyin2phone[334].pinyin,"tun");
	strcpy(pinyin2phone[334].shengmu,"t");
	strcpy(pinyin2phone[334].yunmu,"un");
	strcpy(pinyin2phone[335].pinyin,"tuo");
	strcpy(pinyin2phone[335].shengmu,"t");
	strcpy(pinyin2phone[335].yunmu,"uo");
	strcpy(pinyin2phone[336].pinyin,"wa");
	strcpy(pinyin2phone[336].shengmu,"w");
	strcpy(pinyin2phone[336].yunmu,"a");
	strcpy(pinyin2phone[337].pinyin,"wai");
	strcpy(pinyin2phone[337].shengmu,"w");
	strcpy(pinyin2phone[337].yunmu,"ai");
	strcpy(pinyin2phone[338].pinyin,"wan");
	strcpy(pinyin2phone[338].shengmu,"w");
	strcpy(pinyin2phone[338].yunmu,"an");
	strcpy(pinyin2phone[339].pinyin,"wang");
	strcpy(pinyin2phone[339].shengmu,"w");
	strcpy(pinyin2phone[339].yunmu,"ang");
	strcpy(pinyin2phone[340].pinyin,"wei");
	strcpy(pinyin2phone[340].shengmu,"w");
	strcpy(pinyin2phone[340].yunmu,"ei");
	strcpy(pinyin2phone[341].pinyin,"wen");
	strcpy(pinyin2phone[341].shengmu,"w");
	strcpy(pinyin2phone[341].yunmu,"en");
	strcpy(pinyin2phone[342].pinyin,"weng");
	strcpy(pinyin2phone[342].shengmu,"w");
	strcpy(pinyin2phone[342].yunmu,"eng");
	strcpy(pinyin2phone[343].pinyin,"wo");
	strcpy(pinyin2phone[343].shengmu,"w");
	strcpy(pinyin2phone[343].yunmu,"o");
	strcpy(pinyin2phone[344].pinyin,"wu");
	strcpy(pinyin2phone[344].shengmu,"w");
	strcpy(pinyin2phone[344].yunmu,"u");
	strcpy(pinyin2phone[345].pinyin,"xi");
	strcpy(pinyin2phone[345].shengmu,"x");
	strcpy(pinyin2phone[345].yunmu,"i");
	strcpy(pinyin2phone[346].pinyin,"xia");
	strcpy(pinyin2phone[346].shengmu,"x");
	strcpy(pinyin2phone[346].yunmu,"ia");
	strcpy(pinyin2phone[347].pinyin,"xian");
	strcpy(pinyin2phone[347].shengmu,"x");
	strcpy(pinyin2phone[347].yunmu,"ian");
	strcpy(pinyin2phone[348].pinyin,"xiang");
	strcpy(pinyin2phone[348].shengmu,"x");
	strcpy(pinyin2phone[348].yunmu,"iang");
	strcpy(pinyin2phone[349].pinyin,"xiao");
	strcpy(pinyin2phone[349].shengmu,"x");
	strcpy(pinyin2phone[349].yunmu,"iao");
	strcpy(pinyin2phone[350].pinyin,"xie");
	strcpy(pinyin2phone[350].shengmu,"x");
	strcpy(pinyin2phone[350].yunmu,"ie");
	strcpy(pinyin2phone[351].pinyin,"xin");
	strcpy(pinyin2phone[351].shengmu,"x");
	strcpy(pinyin2phone[351].yunmu,"in");
	strcpy(pinyin2phone[352].pinyin,"xing");
	strcpy(pinyin2phone[352].shengmu,"x");
	strcpy(pinyin2phone[352].yunmu,"ing");
	strcpy(pinyin2phone[353].pinyin,"xiong");
	strcpy(pinyin2phone[353].shengmu,"x");
	strcpy(pinyin2phone[353].yunmu,"iong");
	strcpy(pinyin2phone[354].pinyin,"xiu");
	strcpy(pinyin2phone[354].shengmu,"x");
	strcpy(pinyin2phone[354].yunmu,"iu");
	strcpy(pinyin2phone[355].pinyin,"xu");
	strcpy(pinyin2phone[355].shengmu,"x");
	strcpy(pinyin2phone[355].yunmu,"ux");
	strcpy(pinyin2phone[356].pinyin,"xuan");
	strcpy(pinyin2phone[356].shengmu,"x");
	strcpy(pinyin2phone[356].yunmu,"uxan");
	strcpy(pinyin2phone[357].pinyin,"xue");
	strcpy(pinyin2phone[357].shengmu,"x");
	strcpy(pinyin2phone[357].yunmu,"uxe");
	strcpy(pinyin2phone[358].pinyin,"xun");
	strcpy(pinyin2phone[358].shengmu,"x");
	strcpy(pinyin2phone[358].yunmu,"uxn");
	strcpy(pinyin2phone[359].pinyin,"ya");
	strcpy(pinyin2phone[359].shengmu,"y");
	strcpy(pinyin2phone[359].yunmu,"a");
	strcpy(pinyin2phone[360].pinyin,"yan");
	strcpy(pinyin2phone[360].shengmu,"y");
	strcpy(pinyin2phone[360].yunmu,"an");
	strcpy(pinyin2phone[361].pinyin,"yang");
	strcpy(pinyin2phone[361].shengmu,"y");
	strcpy(pinyin2phone[361].yunmu,"ang");
	strcpy(pinyin2phone[362].pinyin,"yao");
	strcpy(pinyin2phone[362].shengmu,"y");
	strcpy(pinyin2phone[362].yunmu,"ao");
	strcpy(pinyin2phone[363].pinyin,"ye");
	strcpy(pinyin2phone[363].shengmu,"y");
	strcpy(pinyin2phone[363].yunmu,"ie");
	strcpy(pinyin2phone[364].pinyin,"yi");
	strcpy(pinyin2phone[364].shengmu,"y");
	strcpy(pinyin2phone[364].yunmu,"i");
	strcpy(pinyin2phone[365].pinyin,"yin");
	strcpy(pinyin2phone[365].shengmu,"y");
	strcpy(pinyin2phone[365].yunmu,"in");
	strcpy(pinyin2phone[366].pinyin,"ying");
	strcpy(pinyin2phone[366].shengmu,"y");
	strcpy(pinyin2phone[366].yunmu,"ing");
	strcpy(pinyin2phone[367].pinyin,"yo");
	strcpy(pinyin2phone[367].shengmu,"y");
	strcpy(pinyin2phone[367].yunmu,"o");
	strcpy(pinyin2phone[368].pinyin,"yong");
	strcpy(pinyin2phone[368].shengmu,"y");
	strcpy(pinyin2phone[368].yunmu,"ong");
	strcpy(pinyin2phone[369].pinyin,"you");
	strcpy(pinyin2phone[369].shengmu,"y");
	strcpy(pinyin2phone[369].yunmu,"ou");
	strcpy(pinyin2phone[370].pinyin,"yu");
	strcpy(pinyin2phone[370].shengmu,"uxs");
	strcpy(pinyin2phone[370].yunmu,"ux");
	strcpy(pinyin2phone[371].pinyin,"yuan");
	strcpy(pinyin2phone[371].shengmu,"uxs");
	strcpy(pinyin2phone[371].yunmu,"uxan");
	strcpy(pinyin2phone[372].pinyin,"yue");
	strcpy(pinyin2phone[372].shengmu,"uxs");
	strcpy(pinyin2phone[372].yunmu,"uxe");
	strcpy(pinyin2phone[373].pinyin,"yun");
	strcpy(pinyin2phone[373].shengmu,"uxs");
	strcpy(pinyin2phone[373].yunmu,"uxn");
	strcpy(pinyin2phone[374].pinyin,"za");
	strcpy(pinyin2phone[374].shengmu,"z");
	strcpy(pinyin2phone[374].yunmu,"a");
	strcpy(pinyin2phone[375].pinyin,"zai");
	strcpy(pinyin2phone[375].shengmu,"z");
	strcpy(pinyin2phone[375].yunmu,"ai");
	strcpy(pinyin2phone[376].pinyin,"zan");
	strcpy(pinyin2phone[376].shengmu,"z");
	strcpy(pinyin2phone[376].yunmu,"an");
	strcpy(pinyin2phone[377].pinyin,"zang");
	strcpy(pinyin2phone[377].shengmu,"z");
	strcpy(pinyin2phone[377].yunmu,"ang");
	strcpy(pinyin2phone[378].pinyin,"zao");
	strcpy(pinyin2phone[378].shengmu,"z");
	strcpy(pinyin2phone[378].yunmu,"ao");
	strcpy(pinyin2phone[379].pinyin,"ze");
	strcpy(pinyin2phone[379].shengmu,"z");
	strcpy(pinyin2phone[379].yunmu,"e");
	strcpy(pinyin2phone[380].pinyin,"zei");
	strcpy(pinyin2phone[380].shengmu,"z");
	strcpy(pinyin2phone[380].yunmu,"ei");
	strcpy(pinyin2phone[381].pinyin,"zen");
	strcpy(pinyin2phone[381].shengmu,"z");
	strcpy(pinyin2phone[381].yunmu,"en");
	strcpy(pinyin2phone[382].pinyin,"zeng");
	strcpy(pinyin2phone[382].shengmu,"z");
	strcpy(pinyin2phone[382].yunmu,"eng");
	strcpy(pinyin2phone[383].pinyin,"zha");
	strcpy(pinyin2phone[383].shengmu,"zh");
	strcpy(pinyin2phone[383].yunmu,"a");
	strcpy(pinyin2phone[384].pinyin,"zhai");
	strcpy(pinyin2phone[384].shengmu,"zh");
	strcpy(pinyin2phone[384].yunmu,"ai");
	strcpy(pinyin2phone[385].pinyin,"zhan");
	strcpy(pinyin2phone[385].shengmu,"zh");
	strcpy(pinyin2phone[385].yunmu,"an");
	strcpy(pinyin2phone[386].pinyin,"zhang");
	strcpy(pinyin2phone[386].shengmu,"zh");
	strcpy(pinyin2phone[386].yunmu,"ang");
	strcpy(pinyin2phone[387].pinyin,"zhao");
	strcpy(pinyin2phone[387].shengmu,"zh");
	strcpy(pinyin2phone[387].yunmu,"ao");
	strcpy(pinyin2phone[388].pinyin,"zhe");
	strcpy(pinyin2phone[388].shengmu,"zh");
	strcpy(pinyin2phone[388].yunmu,"e");
	strcpy(pinyin2phone[389].pinyin,"zhei");
	strcpy(pinyin2phone[389].shengmu,"zh");
	strcpy(pinyin2phone[389].yunmu,"ei");
	strcpy(pinyin2phone[390].pinyin,"zhen");
	strcpy(pinyin2phone[390].shengmu,"zh");
	strcpy(pinyin2phone[390].yunmu,"en");
	strcpy(pinyin2phone[391].pinyin,"zheng");
	strcpy(pinyin2phone[391].shengmu,"zh");
	strcpy(pinyin2phone[391].yunmu,"eng");
	strcpy(pinyin2phone[392].pinyin,"zhi");
	strcpy(pinyin2phone[392].shengmu,"zh");
	strcpy(pinyin2phone[392].yunmu,"ib");
	strcpy(pinyin2phone[393].pinyin,"zhong");
	strcpy(pinyin2phone[393].shengmu,"zh");
	strcpy(pinyin2phone[393].yunmu,"ong");
	strcpy(pinyin2phone[394].pinyin,"zhou");
	strcpy(pinyin2phone[394].shengmu,"zh");
	strcpy(pinyin2phone[394].yunmu,"ou");
	strcpy(pinyin2phone[395].pinyin,"zhu");
	strcpy(pinyin2phone[395].shengmu,"zh");
	strcpy(pinyin2phone[395].yunmu,"u");
	strcpy(pinyin2phone[396].pinyin,"zhua");
	strcpy(pinyin2phone[396].shengmu,"zh");
	strcpy(pinyin2phone[396].yunmu,"ua");
	strcpy(pinyin2phone[397].pinyin,"zhuai");
	strcpy(pinyin2phone[397].shengmu,"zh");
	strcpy(pinyin2phone[397].yunmu,"uai");
	strcpy(pinyin2phone[398].pinyin,"zhuan");
	strcpy(pinyin2phone[398].shengmu,"zh");
	strcpy(pinyin2phone[398].yunmu,"uan");
	strcpy(pinyin2phone[399].pinyin,"zhuang");
	strcpy(pinyin2phone[399].shengmu,"zh");
	strcpy(pinyin2phone[399].yunmu,"uang");
	strcpy(pinyin2phone[400].pinyin,"zhui");
	strcpy(pinyin2phone[400].shengmu,"zh");
	strcpy(pinyin2phone[400].yunmu,"ui");
	strcpy(pinyin2phone[401].pinyin,"zhun");
	strcpy(pinyin2phone[401].shengmu,"zh");
	strcpy(pinyin2phone[401].yunmu,"un");
	strcpy(pinyin2phone[402].pinyin,"zhuo");
	strcpy(pinyin2phone[402].shengmu,"zh");
	strcpy(pinyin2phone[402].yunmu,"uo");
	strcpy(pinyin2phone[403].pinyin,"zi");
	strcpy(pinyin2phone[403].shengmu,"z");
	strcpy(pinyin2phone[403].yunmu,"if");
	strcpy(pinyin2phone[404].pinyin,"zong");
	strcpy(pinyin2phone[404].shengmu,"z");
	strcpy(pinyin2phone[404].yunmu,"ong");
	strcpy(pinyin2phone[405].pinyin,"zou");
	strcpy(pinyin2phone[405].shengmu,"z");
	strcpy(pinyin2phone[405].yunmu,"ou");
	strcpy(pinyin2phone[406].pinyin,"zu");
	strcpy(pinyin2phone[406].shengmu,"z");
	strcpy(pinyin2phone[406].yunmu,"u");
	strcpy(pinyin2phone[407].pinyin,"zuan");
	strcpy(pinyin2phone[407].shengmu,"z");
	strcpy(pinyin2phone[407].yunmu,"uan");
	strcpy(pinyin2phone[408].pinyin,"zui");
	strcpy(pinyin2phone[408].shengmu,"z");
	strcpy(pinyin2phone[408].yunmu,"ui");
	strcpy(pinyin2phone[409].pinyin,"zun");
	strcpy(pinyin2phone[409].shengmu,"z");
	strcpy(pinyin2phone[409].yunmu,"un");
	strcpy(pinyin2phone[410].pinyin,"zuo");
	strcpy(pinyin2phone[410].shengmu,"z");
	strcpy(pinyin2phone[410].yunmu,"uo");
}


/*
	szm 修改后的结果 
*/
void PrintLabel(TtsLabelCharInfo * cif, short sNum, char *fname)
{
	int i, k;
	FILE *fp;
	fp = fopen(fname, "w");
	//   p1^p2-p3+p4=p5@p6_p7/A:a1_a2_a3/B:b1-b2-b3@b4-b5&b6-b7#b8-b9$b10-b11!b12-b13;b14-b15|b16/C:c1+c2+c3
	//   /D:d1_d2/E:e1+e2@e3+e4&e5+e6#e7+e8/F:f1_f2/G:g1

	////ori-105 bak
	//fprintf(fp, "nu^nu-sil+%s=%s@x_x/A:x_x_x/B:x-x-x@x-x&x-x#x-x$x-x!", cif[0].shengmu, cif[0].yunmu);
	//fprintf(fp, "0-%d;x-x|x/C:x+x+x/D:x_x/E:0+%d@x+x&x+x", cif[0].CharInSentNum, cif[0].PwInSentNum);
	//fprintf(fp, "#0+%d/F:0_0/G:%d\n", cif[0].PpInSentNum, cif[0].IpInSentNum);

	
	//// todo
	fprintf(fp, "nu^nu-sil+%s=%s@x_x", cif[0].shengmu, cif[0].yunmu);
	// 前后音节的调型    x x sil char1 char2 
	fprintf(fp, "/A:x_x_x/B:%d-%d", cif[0].yindiao, cif[1].yindiao);
	// 音节在韵律词、韵律短语、语调短语、句子 的位置（左右）  
	fprintf(fp, "-1@1-1&1-1#1-1$1");
	// 韵律词、韵律短语、语调短语、句子 包含音节数
	fprintf(fp, "-1!1-1;%d", cif[0].CharInSentNum);
	// 韵律词在韵律短语、语调短语、句子 的位置（左右）
	fprintf(fp, "-x|x/C:x+x+x/D:x_x/E:1+%d@x+x&x+x", cif[0].PwInSentNum);
	//fprintf(fp, "-1|1/C:1+1+0/D:%d_x/E:1+%d@x+x&x+x", cif[0].PpInSentNum + 1, cif[0].PwInSentNum);
	
	fprintf(fp, "#1+%d/F:1_1/G:%d\n", cif[0].PpInSentNum, cif[0].IpInSentNum);
	
	for (i = 0; i<sNum; i++)
	{
		// 每一个 音节
		for (k = 1; k <= 2; k++)
		{
			// 声母 韵母 
			//p1
			if (k == 1)
			{
				// 声母
				if (i == 0)
				{
					// 第一个char 
					fprintf(fp, "nu");
				}
				else
				{
					if (cif[i].CharInIpPos == 1)
					{
						fprintf(fp, "%s", cif[i - 1].yunmu);
					}
					else
					{
						fprintf(fp, "%s", cif[i - 1].shengmu);
					}
				}
			}
			else
			{
				// 韵母 
				if (i == 0)
				{
					// 第一个char 
					fprintf(fp, "sil");
				}
				else
				{
					if (cif[i].CharInIpPos == 1)
					{
						fprintf(fp, "sil");
					}
					else
					{
						fprintf(fp, "%s", cif[i - 1].yunmu);
					}
				}
			}
			//p2
			if (k == 1)
			{
				if (i == 0)
				{
					fprintf(fp, "^sil");
				}
				else
				{
					if (cif[i].CharInIpPos == 1)
					{
						fprintf(fp, "^sil");
					}
					else
					{
						fprintf(fp, "^%s", cif[i - 1].yunmu);
					}
				}
			}
			else
			{
				fprintf(fp, "^%s", cif[i].shengmu);
			}
			//p3
			if (k == 1)
			{
				fprintf(fp, "-%s", cif[i].shengmu);
			}
			else
			{
				fprintf(fp, "-%s", cif[i].yunmu);
			}
			//p4
			if (k == 1){
				fprintf(fp, "+%s", cif[i].yunmu);
			}
			else
			{
				if (i<sNum - 1){
					if (cif[i].CharInIpPos == cif[i].CharInIpNum){
						fprintf(fp, "+sil");
					}
					else{
						fprintf(fp, "+%s", cif[i + 1].shengmu);
					}
				}
				else{
					fprintf(fp, "+sil");
				}
			}
			//p5
			if (k == 1)
			{
				if (i<sNum - 1){
					if (cif[i].CharInIpPos == cif[i].CharInIpNum){
						fprintf(fp, "=sil");
					}
					else{
						fprintf(fp, "=%s", cif[i + 1].shengmu);
					}
				}
				else{
					fprintf(fp, "=sil");
				}
			}
			else
			{
				if (i<sNum - 1){
					if (cif[i].CharInIpPos == cif[i].CharInIpNum){
						fprintf(fp, "=%s", cif[i + 1].shengmu);
					}
					else{
						fprintf(fp, "=%s", cif[i + 1].yunmu);
					}
				}
				else{
					fprintf(fp, "=nu");
				}
			}
			//p6
			fprintf(fp, "@%d", k);
			//p7
			if (k == 1)
			{
				fprintf(fp, "_%d", 1 - isQing(cif[i].shengmu));
			}
			else{
				fprintf(fp, "_%d", 1 - isQing(cif[i].yunmu));
			}
			//a1
			if (i >= 2){
				fprintf(fp, "/A:%d", cif[i - 2].yindiao);
			}
			else{
				fprintf(fp, "/A:0");
			}
			//a2
			if (i >= 1){
				fprintf(fp, "_%d", cif[i - 1].yindiao);
			}
			else{
				fprintf(fp, "_0");
			}
			//a3
			fprintf(fp, "_%d", cif[i].yindiao);
			//b1
			if (i<sNum - 1){
				fprintf(fp, "/B:%d", cif[i + 1].yindiao);
			}
			else{
				fprintf(fp, "/B:0");
			}
			//b2
			if (i<sNum - 2){
				fprintf(fp, "-%d", cif[i + 2].yindiao);
			}
			else{
				fprintf(fp, "-0");
			}
			//b3
			fprintf(fp, "-%d", cif[i].CharInPwPos);
			//b4
			fprintf(fp, "@%d", cif[i].CharInPwNum - cif[i].CharInPwPos + 1);
			//b5
			fprintf(fp, "-%d", cif[i].CharInPpPos);
			//b6
			fprintf(fp, "&%d", cif[i].CharInPpNum - cif[i].CharInPpPos + 1);
			//b7
			fprintf(fp, "-%d", cif[i].CharInIpPos);
			//b8
			fprintf(fp, "#%d", cif[i].CharInIpNum - cif[i].CharInIpPos + 1);
			//b9
			fprintf(fp, "-%d", cif[i].CharInSentPos);
			//b10
			fprintf(fp, "$%d", cif[i].CharInSentNum - cif[i].CharInSentPos + 1);
			//b11
			fprintf(fp, "-%d", cif[i].CharInPwNum);
			//b12
			fprintf(fp, "!%d", cif[i].CharInPpNum);
			//b13
			fprintf(fp, "-%d", cif[i].CharInIpNum);
			//b14
			fprintf(fp, ";%d", cif[i].CharInSentNum);
			//b15
			fprintf(fp, "-%d", cif[i].PwInPpPos);
			//b16
			fprintf(fp, "|%d", cif[i].PwInPpNum - cif[i].PwInPpPos + 1);
			//c1
			fprintf(fp, "/C:%d", cif[i].PwInIpPos);
			//c2
			fprintf(fp, "+%d", cif[i].PwInIpNum - cif[i].PwInIpPos + 1);
			//c3
			fprintf(fp, "+%d", cif[i].PwInSentPos);
			//d1
			fprintf(fp, "/D:%d", cif[i].PwInSentNum - cif[i].PwInSentPos + 1);
			//d2
			fprintf(fp, "_%d", cif[i].PwInPpNum);
			//e1
			fprintf(fp, "/E:%d", cif[i].PwInIpNum);
			//e2
			fprintf(fp, "+%d", cif[i].PwInSentNum);
			//e3
			fprintf(fp, "@%d", cif[i].PpInIpPos);
			//e4
			fprintf(fp, "+%d", cif[i].PpInIpNum - cif[i].PpInIpPos + 1);
			//e5
			fprintf(fp, "&%d", cif[i].PpInSentPos);
			//e6
			fprintf(fp, "+%d", cif[i].PpInSentNum - cif[i].PpInSentPos + 1);
			//e7
			fprintf(fp, "#%d", cif[i].PpInIpNum);
			//e8
			fprintf(fp, "+%d", cif[i].PpInSentNum);
			//f1
			fprintf(fp, "/F:%d", cif[i].IpInSentPos);
			//f2
			fprintf(fp, "_%d", cif[i].IpInSentNum - cif[i].IpInSentPos + 1);
			//g1
			fprintf(fp, "/G:%d", cif[i].IpInSentNum);
			//line end
			fprintf(fp, "\n");
			if (cif[i].CharInIpPos == cif[i].CharInIpNum && k == 2)
			{
				fprintf(fp, "%s^%s-sil+", cif[i].shengmu, cif[i].yunmu);
				if (i<sNum - 1){
					fprintf(fp, "%s=%s@x_x/A:x_%d_x/B:%d-x-x@x-x&x-x#x-x$x-x!x-",
						cif[i + 1].shengmu, cif[i + 1].yunmu, cif[i].yindiao, cif[i + 1].yindiao);
				}
				else{
					fprintf(fp, "nu=nu@x_x/A:x_%d_x/B:x-x-x@x-x&x-x#x-x$x-x!x-", cif[i].yindiao);
				}
				fprintf(fp, "%d;%d-x|x/C:x+x+x/D:d1_d2/E:%d+%d@x+x&x+x#", cif[i].CharInIpNum, cif[i].CharInSentNum,
					cif[i].PwInIpNum, cif[i].PwInSentNum);
				fprintf(fp, "%d+%d/F:%d_%d/G:%d\n", cif[i].PpInIpNum, cif[i].PpInSentNum, cif[i].IpInSentPos,
					cif[i].IpInSentNum - cif[i].IpInSentPos + 1, cif[i].IpInSentNum);
			}
		}

	}

	fclose(fp);

}

/*
	ori-105 路程原始备份结果 
*/
void PrintLabel_lucheng(TtsLabelCharInfo * cif,short sNum,char *fname)
{
    int i,k;
    FILE *fp;
    fp=fopen(fname,"w");
    //   p1^p2-p3+p4=p5@p6_p7/A:a1_a2_a3/B:b1-b2-b3@b4-b5&b6-b7#b8-b9$b10-b11!b12-b13;b14-b15|b16/C:c1+c2+c3
    //   /D:d1_d2/E:e1+e2@e3+e4&e5+e6#e7+e8/F:f1_f2/G:g1

	 //ori-105 bak
    fprintf(fp,"nu^nu-sil+%s=%s@x_x/A:x_x_x/B:x-x-x@x-x&x-x#x-x$x-x!",cif[0].shengmu,cif[0].yunmu);
    fprintf(fp,"0-%d;x-x|x/C:x+x+x/D:x_x/E:0+%d@x+x&x+x",cif[0].CharInSentNum,cif[0].PwInSentNum);
    fprintf(fp,"#0+%d/F:0_0/G:%d\n",cif[0].PpInSentNum,cif[0].IpInSentNum);
	
	
	//// todo 
	fprintf(fp, "nu^nu-sil+%s=%s@x_x/A:x_x_x/B:x-x-x@x-x&x-x#x-x$x-x!", cif[0].shengmu, cif[0].yunmu);
	fprintf(fp, "0-x;%d-x|x/C:x+x+x/D:x_x/E:0+%d@x+x&x+x", cif[0].CharInSentNum, cif[0].PwInSentNum);
	fprintf(fp, "#0+%d/F:0_0/G:%d\n", cif[0].PpInSentNum, cif[0].IpInSentNum);
    
	for(i=0;i<sNum;i++)
	{
		// 每一个 音节
        for(k=1;k<=2;k++)
		{
			// 声母 韵母 
            //p1
            if(k==1)
			{
				// 声母
                if(i==0)
				{
					// 第一个char 
                    fprintf(fp,"nu");
                }
                else
				{
                    if(cif[i].CharInIpPos==1)
					{
                        fprintf(fp,"%s",cif[i-1].yunmu);
                    }
                    else
					{
                        fprintf(fp,"%s",cif[i-1].shengmu);
                    }
                }
            }
            else
			{
				// 韵母 
                if(i==0)
				{
					// 第一个char 
                    fprintf(fp,"sil");
                }
                else
				{
                    if(cif[i].CharInIpPos==1)
					{
                        fprintf(fp,"sil");
                    }
                    else
					{
                        fprintf(fp,"%s",cif[i-1].yunmu);
                    }
                }
            }
            //p2
            if(k==1)
			{
                if(i==0)
				{
                    fprintf(fp,"^sil");
                }
                else
				{
                    if(cif[i].CharInIpPos==1)
					{
                        fprintf(fp,"^sil");
                    }
                    else
					{
                        fprintf(fp,"^%s",cif[i-1].yunmu);
                    }
                }
            }
            else
			{
                fprintf(fp,"^%s",cif[i].shengmu);
            }
            //p3
            if(k==1)
			{
                fprintf(fp,"-%s",cif[i].shengmu);
            }
            else
			{
                fprintf(fp,"-%s",cif[i].yunmu);
            }
            //p4
            if(k==1){
                fprintf(fp,"+%s",cif[i].yunmu);
            }
            else
			{
                if(i<sNum-1){
                    if(cif[i].CharInIpPos==cif[i].CharInIpNum){
                        fprintf(fp,"+sil");
                    }
                    else{
                        fprintf(fp,"+%s",cif[i+1].shengmu);
                    }
                }
                else{
                    fprintf(fp,"+sil");
                }
            }
            //p5
            if(k==1)
			{
                if(i<sNum-1){
                    if(cif[i].CharInIpPos==cif[i].CharInIpNum){
                        fprintf(fp,"=sil");
                    }
                    else{
                        fprintf(fp,"=%s",cif[i+1].shengmu);
                    }
                }
                else{
                    fprintf(fp,"=sil");
                }
            }
            else
			{
                if(i<sNum-1){
                    if(cif[i].CharInIpPos==cif[i].CharInIpNum){
                        fprintf(fp,"=%s",cif[i+1].shengmu);
                    }
                    else{
                        fprintf(fp,"=%s",cif[i+1].yunmu);
                    }
                }
                else{
                    fprintf(fp,"=nu");
                }
            }
            //p6
            fprintf(fp,"@%d",k);
            //p7
            if(k==1)
			{
                fprintf(fp,"_%d",1-isQing(cif[i].shengmu));
            }
            else{
                fprintf(fp,"_%d",1-isQing(cif[i].yunmu));
            }
            //a1
            if(i>=2){
                fprintf(fp,"/A:%d",cif[i-2].yindiao);
            }
            else{
                fprintf(fp,"/A:0");
            }
            //a2
            if(i>=1){
                fprintf(fp,"_%d",cif[i-1].yindiao);
            }
            else{
                fprintf(fp,"_0");
            }
            //a3
            fprintf(fp,"_%d",cif[i].yindiao);
            //b1
            if(i<sNum-1){
                fprintf(fp,"/B:%d",cif[i+1].yindiao);
            }
            else{
                fprintf(fp,"/B:0");
            }
            //b2
            if(i<sNum-2){
                fprintf(fp,"-%d",cif[i+2].yindiao);
            }
            else{
                fprintf(fp,"-0");
            }
            //b3
            fprintf(fp,"-%d",cif[i].CharInPwPos);
            //b4
            fprintf(fp,"@%d",cif[i].CharInPwNum - cif[i].CharInPwPos +1 );
            //b5
            fprintf(fp,"-%d",cif[i].CharInPpPos);
            //b6
            fprintf(fp,"&%d",cif[i].CharInPpNum - cif[i].CharInPpPos +1);
            //b7
            fprintf(fp,"-%d",cif[i].CharInIpPos);
            //b8
            fprintf(fp,"#%d",cif[i].CharInIpNum - cif[i].CharInIpPos +1);
            //b9
            fprintf(fp,"-%d",cif[i].CharInSentPos);
            //b10
            fprintf(fp,"$%d",cif[i].CharInSentNum - cif[i].CharInSentPos +1);
            //b11
            fprintf(fp,"-%d",cif[i].CharInPwNum);
            //b12
            fprintf(fp,"!%d",cif[i].CharInPpNum);
            //b13
            fprintf(fp,"-%d",cif[i].CharInIpNum);
            //b14
            fprintf(fp,";%d",cif[i].CharInSentNum);
            //b15
            fprintf(fp,"-%d",cif[i].PwInPpPos);
            //b16
            fprintf(fp,"|%d", cif[i].PwInPpNum- cif[i].PwInPpPos+1 );
            //c1
            fprintf(fp,"/C:%d",cif[i].PwInIpPos);
            //c2
            fprintf(fp,"+%d",cif[i].PwInIpNum - cif[i].PwInIpPos +1);
            //c3
            fprintf(fp,"+%d",cif[i].PwInSentPos);
            //d1
            fprintf(fp,"/D:%d",cif[i].PwInSentNum - cif[i].PwInSentPos +1);
            //d2
            fprintf(fp,"_%d", cif[i].PwInPpNum);
            //e1
            fprintf(fp,"/E:%d", cif[i].PwInIpNum);
            //e2
            fprintf(fp,"+%d", cif[i].PwInSentNum);
            //e3
            fprintf(fp,"@%d", cif[i].PpInIpPos);
            //e4
            fprintf(fp,"+%d", cif[i].PpInIpNum- cif[i].PpInIpPos+1);
            //e5
            fprintf(fp,"&%d", cif[i].PpInSentPos);
            //e6
            fprintf(fp,"+%d", cif[i].PpInSentNum- cif[i].PpInSentPos+1);
            //e7
            fprintf(fp,"#%d", cif[i].PpInIpNum);
            //e8
            fprintf(fp,"+%d", cif[i].PpInSentNum);
            //f1
            fprintf(fp,"/F:%d", cif[i].IpInSentPos);
            //f2
            fprintf(fp,"_%d", cif[i].IpInSentNum- cif[i].IpInSentPos+1);
            //g1
            fprintf(fp,"/G:%d", cif[i].IpInSentNum);
            //line end
            fprintf(fp,"\n");
            if(cif[i].CharInIpPos==cif[i].CharInIpNum && k==2)
			{
                fprintf(fp,"%s^%s-sil+",cif[i].shengmu,cif[i].yunmu);
                if(i<sNum-1 ){
                    fprintf(fp,"%s=%s@x_x/A:x_%d_x/B:%d-x-x@x-x&x-x#x-x$x-x!x-",
                            cif[i+1].shengmu,cif[i+1].yunmu,cif[i].yindiao,cif[i+1].yindiao);
                }
                else{
                    fprintf(fp,"nu=nu@x_x/A:x_%d_x/B:x-x-x@x-x&x-x#x-x$x-x!x-",cif[i].yindiao);
                }
                fprintf(fp,"%d;%d-x|x/C:x+x+x/D:d1_d2/E:%d+%d@x+x&x+x#",cif[i].CharInIpNum,cif[i].CharInSentNum,
                        cif[i].PwInIpNum,cif[i].PwInSentNum);
                fprintf(fp,"%d+%d/F:%d_%d/G:%d\n",cif[i].PpInIpNum,cif[i].PpInSentNum,cif[i].IpInSentPos,
                        cif[i].IpInSentNum-cif[i].IpInSentPos+1,cif[i].IpInSentNum);
            }
        }
    
	}
    
	fclose(fp);

}

