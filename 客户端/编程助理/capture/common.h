/*
	功能:拷贝屏幕矩形区域的像素
	参数:
		lpRect 指定需要拷贝的矩形区域
	返回值:
		指向保存屏幕像素的位图句柄
*/
HBITMAP CopyScreenToBitmap(LPRECT lpRect);
/*
	功能:拷贝设备描述表中指定的矩形区域像素到一个位图当中
	参数:
		lpRect 指定需要拷贝的矩形区域
		hSrcDC 设备描述表句柄
	返回值:
		指向保存位图像素的位图句柄
*/
HBITMAP CopyDCToBitmap(LPRECT lpRect, HDC hSrcDC);