/*
	����:������Ļ�������������
	����:
		lpRect ָ����Ҫ�����ľ�������
	����ֵ:
		ָ�򱣴���Ļ���ص�λͼ���
*/
HBITMAP CopyScreenToBitmap(LPRECT lpRect);
/*
	����:�����豸��������ָ���ľ����������ص�һ��λͼ����
	����:
		lpRect ָ����Ҫ�����ľ�������
		hSrcDC �豸��������
	����ֵ:
		ָ�򱣴�λͼ���ص�λͼ���
*/
HBITMAP CopyDCToBitmap(LPRECT lpRect, HDC hSrcDC);