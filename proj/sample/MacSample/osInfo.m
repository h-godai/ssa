// 
// OSのバージョンとか本体搭載メモリとか調べる
//
#import "osInfo.h"
#import <mach/mach.h>
#import "misc.h"


float osInfoGetVersion(void)
{
	return [[[UIDevice currentDevice] systemVersion] floatValue];
}

NSUInteger osInfoRealMemory(void)
{
	return NSRealMemoryAvailable();
	// シミュレーターだとMac本体の搭載メモリ量を返す:)
}

void osInfoSysMemory(sOSSysMemory *aOut)
{
	struct vm_statistics vm_info;
	mach_msg_type_number_t count = HOST_VM_INFO_COUNT;

	if(host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vm_info, &count) != KERN_SUCCESS)
	{
		PRINTF("%s:Error in host_statistics():%s", __PRETTY_FUNCTION__, strerror(errno));
	}
	else
	{
		aOut->free = vm_info.free_count * vm_page_size;
		aOut->used = (vm_info.wire_count + vm_info.active_count + vm_info.inactive_count) * vm_page_size;
		aOut->unknown = osInfoRealMemory() - (aOut->free + aOut->used);
// 		PRINTF("空き(free_count):%d\n", vm_info.free_count * vm_page_size);
// 		PRINTF("固定中(wire_count):%d\n", vm_info.wire_count * vm_page_size);
// 		PRINTF("使用中(active_count):%d\n", vm_info.active_count * vm_page_size);
// 		PRINTF("未使用(inactive_count):%d\n", vm_info.inactive_count * vm_page_size);
// 		PRINTF("確保中:%d\n", (vm_info.wire_count + vm_info.active_count + vm_info.inactive_count) * vm_page_size);
	}
	
}

NSUInteger osInfoAppMemory(void)
{
	NSUInteger app_mem = 0;

	struct task_basic_info t_info;
	mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

	if(task_info(current_task(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count)!= KERN_SUCCESS)
	{
		PRINTF("%s:Error in task_info():%s", __PRETTY_FUNCTION__, strerror(errno));
	}
	else
	{
		app_mem = t_info.resident_size;
	}
	return app_mem;
}

BOOL osInfoIsCamera(void)
{
	return [UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera];
}
