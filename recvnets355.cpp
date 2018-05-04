/******************************************************************** 
�ļ����� recvnets355.cpp
�����ˣ� handongfeng
��  �ڣ� 2011-02-23
�޸��ˣ� 
��  �ڣ� 
��  ����
��  ���� 
Copyright (c) 2011  YLINK 
********************************************************************/ 

#ifdef _LINUX_
#define SQLCA_STORAGE_CLASS extern
#include "sqlca.h"
#endif
#include "recvnets355.h"

CRecvNets355::CRecvNets355()
{

}

CRecvNets355::~CRecvNets355()
{

}

int CRecvNets355::doWorkSelf(LPCSTR szMsg)
{
    Trace(L_INFO,  __FILE__,  __LINE__, NULL, "ENTER CRecvNets355::Work()");
    
    // ��ȡ��������
    GetWorkDate(m_dbproc, m_sWorkDate, SYS_BEPS);
    m_strWorkDate = m_sWorkDate;    
    int iRet = 0;
    if (NULL == szMsg || '\0' == szMsg)
    {
        Trace(L_ERROR, __FILE__, __LINE__, NULL, "���ĳ���Ϊ��!");
        return OPERACT_FAILED;
    }

    // ��������
    iRet = unPack(szMsg);
    if (OPERACT_SUCCESS != iRet)
    {
        Trace(L_ERROR, __FILE__, __LINE__, NULL, "���Ľ�������!");
        return iRet;
    }

    iRet = SetData(szMsg);
	if (OPERACT_SUCCESS != iRet)
    {
        Trace(L_ERROR,  __FILE__,  __LINE__, NULL, "��䱨��ʧ��");
        printf("��䱨��ʧ��\n");
        return iRet;
    }
        
    // ��������
    iRet = InsertData();
    if (OPERACT_SUCCESS != iRet)
    {
        Trace(L_ERROR,  __FILE__,  __LINE__, NULL, "��������ʧ�� iRet = %d", iRet, m_Sartrlmt.GetSqlErr());
        return iRet;
    }    

    Trace(L_INFO,  __FILE__,  __LINE__, NULL, "LEAVE CRecvNets355::Work()");
    return OPERACT_SUCCESS;
}

INT32 CRecvNets355::unPack(LPCSTR szMsg)
{
    Trace(L_INFO,  __FILE__,  __LINE__, NULL, "ENTER CRecvNets355::unPack()");
    int iRet = -1;

    m_parser355.Init();

    // ��������
    if (OPERACT_SUCCESS != m_parser355.ParseXml(szMsg))
    {
        Trace(L_ERROR,  __FILE__,  __LINE__, NULL, "���Ľ�������! iRet= %d", iRet);	
        return iRet;
    }
    
    Trace(L_INFO,  __FILE__,  __LINE__, NULL, "LEAVE CRecvNets355::unPack()");
    return OPERACT_SUCCESS;
}

INT32 CRecvNets355::InsertData()
{
    Trace(L_INFO,  __FILE__,  __LINE__, NULL, "ENTER CRecvNets355::InsertData()");
    
    SETCTX(m_Sartrlmt);
    int iRet = m_Sartrlmt.insert();

    Trace(L_INFO,  __FILE__,  __LINE__, NULL, "LEAVE CRecvNets355::InsertData()  iRet=%d", iRet);
    return iRet;
}

INT32 CRecvNets355::SetData(LPCSTR pchMsg)
{
    Trace(L_INFO,  __FILE__,  __LINE__, NULL, "ENTER CRecvNets355::SetData()");

    m_Sartrlmt.m_msgid = m_parser355.MsgIdId; 
    m_Sartrlmt.m_workdate = m_strWorkDate ; 
    m_Sartrlmt.m_msgtp = "nets.355.001.01";
    m_Sartrlmt.m_ccy   = m_parser355.CurLmtCcy;
    m_Sartrlmt.m_dmstacctid = m_parser355.DmstAcctId;
    m_Sartrlmt.m_amtwthccy  = atof(m_parser355.CurLmtAmtWthCcy.c_str());
    m_Sartrlmt.m_indsts     = m_parser355.Sts;
    m_Sartrlmt.m_dfltlmt    = m_parser355.DfltLmtAmtWthCcy;

    //add by zwc 20180504 �յ�nets.355�����ʼ���ָ���û�
    char cMail[4096] = {0};
    strcpy(cMail,pchMsg);
    Trace(L_INFO,  __FILE__,  __LINE__, NULL, "mail msg\n[%s]", cMail);

	char MailUserName[255] = {0};
	char MailSmtpServer[255] = {0};
	char MailSendfrom[255] = {0};
	char MailSendTo[255] = {0};
	char MailPort[255] = {0};
	char MailPro[255] = {0};

	int iRet = GetSysParam(m_dbproc, "41", MailUserName);
	if (OPERACT_SUCCESS != iRet)
	{
		Trace(L_ERROR,	__FILE__,  __LINE__, NULL, "Get Parm [41:MailUserName] faild");
		exit(0);
	}

    iRet = GetSysParam(m_dbproc, "43", MailSmtpServer);
	if (OPERACT_SUCCESS != iRet)
	{
		Trace(L_ERROR,	__FILE__,  __LINE__, NULL, "Get Parm [43:MailSmtpServer] faild");
	}	
    iRet = GetSysParam(m_dbproc, "44", MailSendfrom);
	if (OPERACT_SUCCESS != iRet)
	{
		Trace(L_ERROR,	__FILE__,  __LINE__, NULL, "Get Parm [44:MailSendfrom] faild");
	}	
    iRet = GetSysParam(m_dbproc, "48", MailSendTo);
	if (OPERACT_SUCCESS != iRet)
	{
		Trace(L_ERROR,	__FILE__,  __LINE__, NULL, "Get Parm [48:MailSendTo] faild");
	}	
    iRet = GetSysParam(m_dbproc, "46", MailPort);
	if (OPERACT_SUCCESS != iRet)
	{
		Trace(L_ERROR,	__FILE__,  __LINE__, NULL, "Get Parm [46:MailPort] faild");
	}	
	
    iRet = GetSysParam(m_dbproc, "47", MailPro);
	if (OPERACT_SUCCESS != iRet)
	{
		Trace(L_ERROR,	__FILE__,  __LINE__, NULL, "Get Parm [46:MailPro] faild")
	}	
		

	char MailSendCmd[4096] = {0};
	sprintf(MailSendCmd,"%s \"%s\" \"\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s�յ�������޶���ö��Ԥ��֪ͨ����,MsgID=[%s]\" \"%s\" ",
						MailPro,
		                MailUserName,
		                MailSmtpServer,
		                MailPort,
		                MailSendfrom,
		                MailSendTo,
		                m_strWorkDate.c_str(),
		                m_parser355.MsgIdId.c_str()
		                cMail);

	Trace(L_ERROR,	__FILE__,  __LINE__, NULL, "MailSendCmd [%s]",MailSendCmd);

	iRet = system(MailSendCmd);
	if (0 != iRet)
	{
		Trace(L_ERROR,	__FILE__,  __LINE__, NULL, "mail send failed iRet=[%d]",iRet);
	}

	Trace(L_INFO,	__FILE__,  __LINE__, NULL, "mail send success");
    //add end zwc 20180504 �յ�nets.355�����ʼ���ָ���û�

    Trace(L_INFO,  __FILE__,  __LINE__, NULL, "LEAVE CRecvNets355::SetData()");
    return OPERACT_SUCCESS;
}

