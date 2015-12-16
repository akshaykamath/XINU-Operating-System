#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#if FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;

#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode)) // 512/ 64 = 8
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1) // 128 / 8 = 16 blocks
#define FIRST_INODE_BLOCK 2
#define FIRST_DATA_BLOCK (FIRST_INODE_BLOCK + NUM_INODE_BLOCKS + 1)
// custom section

// Create a static global inode entry to get the next inode number
// It should be incremented each time a new inode has been assigned to a file
static int nextInodeEntry = 0;

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

int fs_create(char *filename, int mode)
{	 
	struct dirent *fileEntry;
	struct directory *rootDirectory = &fsd.root_dir;
	struct filetable *ftEntry;	
	struct inode in;
	
	if(rootDirectory == NULL)
	{
		printf("Root Dir is null\n");
	}

	if (strlen(filename) > FILENAMELEN) 
	{
		printf("File name too long.: %d\n",FILENAMELEN);
		return SYSERR;			
	}

	// Only accept create flag 
	if (mode != O_CREAT)
	{
		printf("File flag should be set to create.\n");
		return SYSERR;
	}

	// Check if we can still create a directory..the limit is 16
	if (rootDirectory->numentries >= DIRECTORY_SIZE)
	{
		printf("The directory limit is reached.");
	}

	// Check if an inode is available, if not return
	if(nextInodeEntry > fsd.ninodes)
	{
		printf("No Inodes available.\n");
		return SYSERR;
	}
	
	// Check if file name is present in the directory
	int i;
	for (i = 0; i < DIRECTORY_SIZE; i++) 
	{
		fileEntry = &rootDirectory->entry[i];		
		if (strncmp(fileEntry->name, filename, FILENAMELEN) == 0)
		{
			printf("File %s already exists.\n", filename);
			return SYSERR;
		}
	}

	// Create a new entry in the root directory
	strncpy(&fsd.root_dir.entry[fsd.root_dir.numentries].name, filename, FILENAMELEN-1);
	fsd.root_dir.entry[fsd.root_dir.numentries].name[FILENAMELEN -1] = '\0'; 
	fsd.root_dir.entry[fsd.root_dir.numentries].inode_num = -999;	
	fsd.root_dir.numentries += 1;

	int currentFd = fs_open(fsd.root_dir.entry[fsd.root_dir.numentries-1].name, O_RDWR);

	printf("\nFile %s created and opened successfully.\n",fileEntry->name);

	return currentFd;
}


int fs_open(char *filename, int flags)
{
	struct dirent *fileEntry;
	struct directory *rootDirectory = &fsd.root_dir;
	struct filetable *ftEntry;
	struct inode in;

	// The flag should be a valid flag
	if (flags != O_RDONLY && flags != O_WRONLY && flags != O_RDWR)	
	{
		printf("File flag not supported.\n");
		return SYSERR;
	}

	// File should be present in the directory
	// If not, return error as there is no file to update
	int i;
	int isPresentInDirectory = 0;
	int entryIndex = 0;
	int inodeNum = 0;
	for (i = 0; i < DIRECTORY_SIZE; i++) 
	{
		fileEntry = &rootDirectory->entry[i];

		if (strncmp(fileEntry->name, filename, FILENAMELEN - 1) == 0)
		{
			printf("file present in dir; name: %s\t comare str: %s\n", fileEntry->name, filename);
			isPresentInDirectory = 1;	
			inodeNum = fileEntry->inode_num;
			break;
		}

		entryIndex++;
	}
	
	if(isPresentInDirectory == 0)
	{
		printf("File does not exist in the file table. Please create the file first.");
		return SYSERR;
	}
	
	// If there is a chance that the file is in the directory but not in the file table entry (not possible with the given requirements), 
	// create a new entry or use a closed entry.
	// we use a flag to identify if the file is in the open file table entry
	int isPresentftEntry = 0;
	for(i = 0; i < NUM_FD; i++)
	{
 		ftEntry = &oft[i];
		if(ftEntry->in.id == inodeNum)
		{
			isPresentftEntry = 1;
			break;
		}
	}
	
	// If file table entry is not present, check to see if there is a closed file table entry that we can use, else just create a new one.
	if (isPresentftEntry == 0)
	{
		int foundUsable = 0;
		for(i = 0; i < NUM_FD; i++)
		{
 			ftEntry = &oft[i];
			if(ftEntry->state == FSTATE_CLOSED)
			{
				foundUsable = 1;
				break;
			}
		}
		
		if (foundUsable == 0)	
		{
			// Try to create a new entry, but the entry should not be greater the max entries allowed
			if(next_open_fd > NUM_FD)
			{
				printf("Max Number of open file entries reached. Close a few files and then retry?\n");
				return SYSERR;
			}
			
			// Else, just create a new entry in the file table.
			ftEntry = &oft[next_open_fd];
			next_open_fd += 1;
		}
	}

	fs_get_inode_by_num(0, nextInodeEntry, &in);

	// Update the filesystem datastructure with the number of used inodes	

	//fsd.root_dir.entry[entryIndex].inode_num = nextInodeEntry;
	fileEntry->inode_num = nextInodeEntry;
	fsd.inodes_used += 1;

	for (i = 0; i < DIRECTORY_SIZE; i++) 
	{
		fileEntry = &rootDirectory->entry[i];
		if (strncmp(fileEntry->name, filename, FILENAMELEN - 1) == 0)
		{
			printf("File: %s\t INODE: %d\n", fileEntry->name, fileEntry->inode_num);
		}
	}
	
	// Create a new entry in the open file table
	//ftEntry = &oft[next_open_fd];
	ftEntry->state = FSTATE_OPEN;
	ftEntry->file_flag = O_RDWR;
	ftEntry->de = fileEntry;
	ftEntry->in = in;
	ftEntry->in.id = nextInodeEntry;
	ftEntry->in.type = INODE_TYPE_FILE;
	ftEntry->in.nlink = 0;
	ftEntry->in.device = 0;
	ftEntry->in.size = 0;

	int currentFd = next_open_fd;
	
	// Update the next_open_fd
	next_open_fd += 1;
	nextInodeEntry += 1;	

	// Update the inode entry in the datastore
	fs_put_inode_by_num(dev0, ftEntry->in.id, &ftEntry->in);	

	// printf("\nFile %s opened successfully.\n",fileEntry->name);
	return currentFd;
}

int fs_close(int fd)
{
	struct filetable *ftEntry = &oft[fd];
	if(ftEntry->state == FSTATE_OPEN) 
	{
		printf("Closing file table entry %d\n", fd);
		ftEntry->state = FSTATE_CLOSED;
		return OK;
		
	}

	printf("File is not open\n");
	return SYSERR;
}

int fs_seek(int fd, int offset)
{
	struct filetable *ftEntry = &oft[fd];
	printf("________________________seek_______________________\n");
	// Check file is open
	if(ftEntry->state != FSTATE_OPEN) 
	{
		printf("file is not open\n");
		return SYSERR;	
	}
	printf("Pointer before seek: %d\n", ftEntry->fileptr);
	
	ftEntry->fileptr = ftEntry->fileptr + offset;
	printf("Pointer after seek: %d\n", ftEntry->fileptr);
	printf("________________________seek_______________________\n\n");
	return 0;
}

int fs_read(int fd, void *buf, int nbytes)
{
	int bytesToBeTransferred = nbytes;
	int bytesTransferred = 0;
	int block;
	printf("________________________Read_______________________\n\n");
	// verify arguments
	struct filetable *ftEntry  = &oft[fd];
	
	// Check file 
	if(ftEntry->state != FSTATE_OPEN) 
	{
		printf("file is not open\n");
		return SYSERR;	
	}

	// Check file mode
	if (ftEntry->file_flag != O_RDONLY && ftEntry->file_flag != O_RDWR)
	{
		printf("file not open in read mode: %d\n", ftEntry->mode);
		return SYSERR;			
	}
	
	printf("fileptr in read :%d\n", ftEntry->fileptr);
	int firstblockPointer = ftEntry->fileptr / MDEV_BLOCK_SIZE;
	int firstblock = ftEntry->in.blocks[firstblockPointer];
	int lastblockPointer = ftEntry->in.size / MDEV_BLOCK_SIZE;
	int lastblock = ftEntry->in.blocks[lastblockPointer];
	int bytesToRead = 0, bytesRead = 0;
	int offset = 0;		
	
	//printf("First block = %d, Last Block = %d\n", firstblock, lastblock);
	
	int i = 0;
	// Read only through the required block until the last block	
	for(i = firstblockPointer; i <= lastblockPointer; i++)
	{
		block = ftEntry->in.blocks[i];		
		offset = ftEntry->fileptr % MDEV_BLOCK_SIZE;
				
	
		if(nbytes - bytesRead < MDEV_BLOCK_SIZE)
		{
			bytesToRead = nbytes - bytesRead;
		}
		else
		{
			bytesToRead = MDEV_BLOCK_SIZE - offset;	
		}

		printf("bytes to read: %d\n", bytesToRead);
	
		if (bs_bread(dev0, block, offset, buf, bytesToRead)!=OK) 
		{
			printf("block read failed\n");
			return SYSERR;
		}		
		
		buf = buf + bytesToRead;

		bytesRead += bytesToRead;
		ftEntry->fileptr = bytesRead;				
	}
	printf("\nNumber of Bytes Read %d\n\n", bytesRead);

	printf("________________________Read_______________________\n\n");
	return bytesRead;	
}

int fs_write(int fd, void *buf, int nbytes)
{
	int bytesToBeTransferred = nbytes;
	int bytesTransferred = 0;
	int block;
	int offset;

	// verify arguments
	struct filetable *ftEntry = &oft[fd];
	
	// Check file 
	if(ftEntry->state != FSTATE_OPEN) 
	{
		printf("file is not open\n");
		return SYSERR;	
	}

	// Check file mode
	if (ftEntry->file_flag != O_WRONLY && ftEntry->file_flag != O_RDWR)
	{
		printf("file not open in write mode: %d\n", ftEntry->mode);
		return SYSERR;			
	}	
	
	char *buffer = (char*)	buf;
	// nbytes += 1;		
	// buffer[nbytes] = '\0';

	// First allocate block locations to the inode
	int lastdatablockofcurrentinode = FIRST_DATA_BLOCK + ((ftEntry->in.id + 1) * 12);
	int firstdatablockofcurrentinode = lastdatablockofcurrentinode - 12;
	int inodeLoop = 0;
	int counter = 0;
	void *bytesToFlush;
	for (inodeLoop = firstdatablockofcurrentinode; inodeLoop < lastdatablockofcurrentinode; inodeLoop++)
	{
		ftEntry->in.blocks[counter++] = inodeLoop;
	}

	// Then start the transfer process
	int blockSizeRequired = nbytes/MDEV_BLOCK_SIZE;
	int off = nbytes % MDEV_BLOCK_SIZE;
	if(off > 0)
	{
		blockSizeRequired += 1;
	}
	
	if(blockSizeRequired > INODEBLOCKS)
	{
		printf("Block too big\n");
		return SYSERR;
	}
	int i;
	for(i = 0; i < blockSizeRequired; i++)
	{
		// This is the block to be written
		block = ftEntry->in.blocks[i];
		// Get the number of bytes that will be transferred now

		if(nbytes > MDEV_BLOCK_SIZE)
		{
			bytesToBeTransferred = MDEV_BLOCK_SIZE;
			nbytes = nbytes - MDEV_BLOCK_SIZE;
		}
		else
		{
			bytesToBeTransferred = nbytes;
		}

		bytesToFlush = getmem(bytesToBeTransferred);
		memcpy(bytesToFlush, buffer + bytesTransferred, bytesToBeTransferred);
		//printf("\nWriting %s\n", bytesToFlush);		
		if (bs_bwrite(dev0, block, 0, bytesToFlush, bytesToBeTransferred)!=OK) 
		{
			printf("block write failed\n");
			return SYSERR;
		}
		
		bytesTransferred += bytesToBeTransferred;
		ftEntry->fileptr = bytesTransferred;
		// printf("Updated block %d\n", block);
		fs_setmaskbit(block);
	}
	
	//memcpy(bytesToFlush, buffer + bytesTransferred, bytesToBeTransferred);

	ftEntry->in.size = ftEntry->fileptr;	
	printf("Bytes written count: %d\n", ftEntry->fileptr);
	printf("Bytes written : %s\n", buf);

	fs_put_inode_by_num(dev0, ftEntry->in.id, &ftEntry->in);
	return bytesTransferred;	
}

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);
  fs_setmaskbit(bl);
  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  // Store i = 512
  i = fsd.nblocks;
  
  // Round it off.
  while ( (i % 8) != 0) {i++;}

  // Freemaskbytes = 512/ 8 = 64 bytes. Hence there are 512 bits. Each one of the 64 bytes is a mask that will be set.
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);
  printf("Size of file system: %d\n", sizeof(struct fsystem));
  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
  printf("FIRST DATA BLOCK: %D\n",FIRST_DATA_BLOCK);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

#endif /* FS */

