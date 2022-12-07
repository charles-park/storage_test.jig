//------------------------------------------------------------------------------
/**
 * @file storage_test.h
 * @author charles-park (charles.park@hardkernel.com)
 * @brief storage device test application for odroid-jig (emmc, sd, sata, nvme..)
 * @version 0.1
 * @date 2022-12-07
 *
 * @copyright Copyright (c) 2022
 *
 */
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef __STORAGE_TEST_H__
#define	__STORAGE_TEST_H__

//------------------------------------------------------------------------------
extern	int storage_read_test	(char *dev_node, int count);
extern	int storage_test		(const char *dev_name, char *resp_str);

//------------------------------------------------------------------------------
#endif	//	#ifndef __STORAGE_TEST_H__
//------------------------------------------------------------------------------
