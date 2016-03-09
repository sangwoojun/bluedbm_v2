#include "flashmanager.h"

#include <list>
#include <vector>
#include <string>
#include <map>

// How many block do we want ready at any time?
#define BSBFS_ERASE_PREPARE 16
#define BSBFS_ERASE_INFLIGHT 8


/*
Functional TODO

1.
periodically flush appendbuffer to flash.
But this means if it's appended again after,
the whole block has to be moved
*/

#ifndef __BSBFS_H__
#define __BSBFS_H__

void* blockEraserThread(void* arg);

class File {
public:
	File(std::string name);
	std::string filename;
	void clear();

	uint8_t* appendbuffer;
	uint8_t* readbuffer;
	
//private: //TODO
	std::vector<uint32_t> blockmap;
	uint64_t size; // bytes
	
	//TODO append in the middle, purge after?
	uint64_t seek; // bytes // USED ONLY FOR READS!

public:
	enum {
		FSEEK_SET,
		FSEEK_CUR
	};
	enum {
		ENOENT = 2,
		EEXIST = 17,
		EINVAL = 22
	};
	typedef struct Stat {
		std::string name;
		uint64_t size;
	} Stat;
};

// BlueDBM Simple Blob File System
class BSBFS {
public:
	static BSBFS* getInstance();

	int createFile(std::string filename);
	int deleteFile(std::string filename);
	int open(std::string filename);
	uint64_t fread(int fd, void* ptr, uint64_t size);
	int fseek(int fd, uint64_t offset, int whence);
	int feof(int fd);
	uint64_t ftell(int fd);
	uint64_t fappend(int fd, void* buffer, uint64_t size);
	void fileList();

	int readPage(int fd, uint64_t page, void* buf, uint8_t* stat);
	int writePage(int fd, uint64_t page, void* buf, uint8_t* stat);
	

private:
	static BSBFS* m_pInstance;
	BSBFS();

	std::vector<File*> files;

	pthread_t eraserThread;
	
	void waitBlockExist(int fd, uint32_t bidx);
public:
	uint32_t cur_blockeraseidx;
	std::list<uint32_t> listErased;
	pthread_mutex_t eraseMutex;
	pthread_cond_t eraseCond;

static void pageMap(uint64_t page, PhysPage& np);
static uint32_t blockIdx(uint64_t page);

};

void* blockEraserThread(void* arg);
#endif