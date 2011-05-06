#include "fat64.h"

ide_device_t * ide_device;
fat64_partition_t * fat64_partition;

status_t _fat64_init(void) {
	ide_device = &(ide_devices[1]);
	return E_SUCCESS;
}
