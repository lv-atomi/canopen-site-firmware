/*
 * CANopen Object Dictionary storage object (blank example).
 *
 * @file        CO_storageBlank.c
 * @author      Janez Paternoster
 * @copyright   2021 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "CO_storageLittleFS.h"
#include "301/CO_driver.h"
#include "flash.h"
#include "lfs.h"
#include "log.h"

#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE

lfs_t lfs;

/*
 * 存储分配：
 * flash总容量1MB
 * 切成2块，分别用来存放当前版本和下一个升级版本的固件
 * 除此之外还需要存放canopen的persist信息，切128KB出来。
 * 这样总空间如下分配：448KB + 448KB + 128KB
 * 0x08000000 ~ 0x08070000 (0x08000000 + 0x70000)
 * 0x08070000 ~ 0x080e0000 (0x08070000 + 0x70000)
 * 0x080e0000 ~ 0x08100000 (0x080e0000 + 0x20000)
 */
#define FLASH_LITTELFS_STORAGE          0x080e0000
#define FLASH_LITTELFS_STORAGE_SIZE     0x20000

int block_device_read(const struct lfs_config *c, lfs_block_t block,
                      lfs_off_t off, void *buffer, lfs_size_t size) {
  /* log_printf("flash read:%p, len:%ld\n", buffer, size); */
  uint32_t addr = FLASH_LITTELFS_STORAGE + block * c->block_size + off;
  for (size_t i = 0; i < size; i++) {
    *((uint8_t*)buffer + i) = *((uint8_t*)addr + i);
  }
  return 0;
}

int block_device_prog(const struct lfs_config *c, lfs_block_t block,
                      lfs_off_t off, const void *buffer, lfs_size_t size) {
  
  /* log_printf("flash prog:%p, len:%ld\n", buffer, size); */
  uint32_t addr = FLASH_LITTELFS_STORAGE + block * c->block_size + off;
  const uint8_t *byte_buffer = (const uint8_t *)buffer;
  uint32_t i = 0;

  flash_bank2_unlock();  //解锁flash，允许写入操作

  //处理头部的非对齐部分
  while ((addr + i) % 4 != 0 && i < size) {
    if (flash_byte_program(addr + i, byte_buffer[i]) != FLASH_OPERATE_DONE) {
      flash_bank2_lock();  // 锁定flash，防止更多的写入操作
      /* log_printf("flash prog error\n"); */
      return LFS_ERR_CORRUPT;
    }
    i++;
  }

  //处理对齐部分
  for (; i < size - (size - i) % 4; i+=4) {
    if (flash_word_program(addr + i, *((uint32_t *)(byte_buffer + i))) != FLASH_OPERATE_DONE) {
      flash_bank2_lock();  // 锁定flash，防止更多的写入操作
      /* log_printf("flash prog error2\n"); */
      return LFS_ERR_CORRUPT;
    }
  }

  //处理尾部的非对齐部分
  while (i < size) {
    if (flash_byte_program(addr + i, byte_buffer[i]) != FLASH_OPERATE_DONE) {
      flash_bank2_lock();  // 锁定flash，防止更多的写入操作
      /* log_printf("flash prog error3\n"); */
	    
      return LFS_ERR_CORRUPT;
    }
    i++;
  }

  flash_bank2_lock();  // 锁定flash，防止更多的写入操作
  return 0;
}

int block_device_erase(const struct lfs_config *c, lfs_block_t block) {
  uint32_t addr = FLASH_BANK1_START_ADDR + block * c->block_size;
  /* log_printf("flash erase, block at:%p, size:%ld\n", (void*)(intptr_t)(block * c->block_size), c->block_size); */

  flash_bank2_unlock();  //解锁flash，允许擦除操作
  if (flash_sector_erase(addr) != FLASH_OPERATE_DONE) {
    flash_bank2_lock();  // 锁定flash，防止更多的擦除操作
    /* log_printf("flash erase error\n"); */

    return LFS_ERR_CORRUPT;
  }
  flash_bank2_lock();  // 锁定flash，防止更多的擦除操作
  
  return 0;
}

int block_device_sync(const struct lfs_config *c) {
  // 对于AT32的Flash，我们可能并不需要做任何同步操作，因为写入和擦除操作应该是立即生效的。
  // 如果你的硬件有特别的同步需求，你可以在这里实现它。
  return 0;
}

uint8_t lfs_read_file_buffer[64];
uint8_t uploadFileName[64];
uint8_t lfs_read_buffer[64];
uint8_t lfs_prog_buffer[64];
uint8_t lfs_lookahead_buffer[64];

// Configuration of the filesystem
const struct lfs_config cfg = {
    .read = block_device_read,
    .prog = block_device_prog,
    .erase = block_device_erase,
    .sync = block_device_sync,
    .read_size = 2,
    .prog_size = 2,
    .block_size = 2048,
    .block_count = 64,
    .cache_size = 64,
    .lookahead_size = 32,
    .read_buffer = lfs_read_buffer,
    .prog_buffer = lfs_prog_buffer,
    .lookahead_buffer = lfs_lookahead_buffer,
    .block_cycles = 10000,
};
/*
 * Function for writing data on "Store parameters" command - OD object 1010
 *
 * For more information see file CO_storage.h, CO_storage_entry_t.
 */
static ODR_t storeLittleFS(CO_storage_entry_t *entry,
                           CO_CANmodule_t *CANmodule) {
  lfs_file_t file;
  struct lfs_file_config uploadFileConfig;


  log_printf("store:%d\n", entry->len);
  
  memset(&uploadFileConfig, 0, sizeof(struct lfs_file_config));
  uploadFileConfig.buffer = lfs_read_file_buffer;  // use the static buffer
  uploadFileConfig.attr_count = 0;
  
  CO_LOCK_OD(CANmodule);

  if (lfs_file_opencfg(&lfs, &file, (char*) entry->filename,
		       LFS_O_RDWR | LFS_O_CREAT, &uploadFileConfig) != LFS_ERR_OK){
  
  /* if (lfs_file_open(&lfs, &file, entry->filename, */
  /*                   LFS_O_RDWR | LFS_O_CREAT) != LFS_ERR_OK) { */
    CO_UNLOCK_OD(CANmodule);
    return ODR_HW;
  }

  if (lfs_file_write(&lfs, &file, entry->addr, entry->len) != entry->len) {
    lfs_file_close(&lfs, &file);
    CO_UNLOCK_OD(CANmodule);
    return ODR_HW;
  }

  lfs_file_close(&lfs, &file);

  CO_UNLOCK_OD(CANmodule);

  return ODR_OK;
}

/*
 * Function for restoring data on "Restore default parameters" command - OD 1011
 *
 * For more information see file CO_storage.h, CO_storage_entry_t.
 */
static ODR_t restoreLittleFS(CO_storage_entry_t *entry,
                             CO_CANmodule_t *CANmodule) {
  lfs_file_t file;

  struct lfs_file_config uploadFileConfig;

  /* log_printf("restore:%d\n", entry->len); */

  memset(&uploadFileConfig, 0, sizeof(struct lfs_file_config));
  uploadFileConfig.buffer = lfs_read_file_buffer;  // use the static buffer
  uploadFileConfig.attr_count = 0;

  CO_LOCK_OD(CANmodule);

  /* if (lfs_file_open(&lfs, &file, entry->filename, */
  /*                   LFS_O_RDONLY) != LFS_ERR_OK) { */
  if (lfs_file_opencfg(&lfs, &file, (char*)entry->filename,
		       LFS_O_RDONLY, &uploadFileConfig) != LFS_ERR_OK) {
    CO_UNLOCK_OD(CANmodule);
    return ODR_HW;
  }

  if (lfs_file_read(&lfs, &file, entry->addr, entry->len) != entry->len) {
    lfs_file_close(&lfs, &file);
    CO_UNLOCK_OD(CANmodule);
    return ODR_HW;
  }

  lfs_file_close(&lfs, &file);

  CO_UNLOCK_OD(CANmodule);

  return ODR_OK;
}

CO_ReturnError_t
CO_storageLittleFS_init(CO_storage_t *storage, CO_CANmodule_t *CANmodule,
                        OD_entry_t *OD_1010_StoreParameters,
                        OD_entry_t *OD_1011_RestoreDefaultParam,
                        CO_storage_entry_t *entries, uint8_t entriesCount,
                        uint32_t *storageInitError) {
  CO_ReturnError_t ret;

  // Mount the filesystem
  int err = lfs_mount(&lfs, &cfg);

  // reformat if we can't mount the filesystem, this should only happen on the
  // first boot
  if (err) {
    log_printf("little fs format! %d\n", err);
    lfs_format(&lfs, &cfg);
    err = lfs_mount(&lfs, &cfg);
    if (err){
      log_printf("little fs format failed! %d\n", err);
      return CO_ERROR_SYSCALL;
    }
  }

  ret = CO_storage_init(storage, CANmodule, OD_1010_StoreParameters,
                        OD_1011_RestoreDefaultParam, storeLittleFS,
                        restoreLittleFS, entries, entriesCount);
  if (ret != CO_ERROR_NO) {
    return ret;
  }

  *storageInitError = 0;
  for (uint8_t i = 0; i < entriesCount; i++) {
    CO_storage_entry_t *entry = &entries[i];

    // verify arguments
    if (entry->addr == NULL || entry->len == 0 || entry->subIndexOD < 2) {
      *storageInitError = i;
      return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    // No action needed here if LittleFS is already mounted
  }

  storage->enabled = true;
  return ret;
}

#endif /* (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE */
