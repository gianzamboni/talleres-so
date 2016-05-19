#include "ext2fs.h"
#include "hdd.h"
#include "pentry.h"
#include "mbr.h"
#include <cassert>
#include <cstring>
#include <iostream>

Ext2FS::Ext2FS(HDD & disk, unsigned char pnumber) : _hdd(disk), _partition_number(pnumber)
{
	assert(pnumber <= 3);

	// Load Superblock
	unsigned char * buffer = new unsigned char[2*SECTOR_SIZE];

	// Get partition info
	const PartitionEntry & pentry = _hdd[_partition_number];

	// Skip first two sectors & read 3rd and 4th sectors
	// Warning: We assume common sector size of 512bytes but it may not be true
	_hdd.read(pentry.start_lba()+2, buffer);
	_hdd.read(pentry.start_lba()+3, buffer+SECTOR_SIZE);

	// Warning: this only works in Little Endian arhitectures
	_superblock = (struct Ext2FSSuperblock *) buffer;

	// Load Block Group Table

	// Compute block size by shifting the value 1024
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;

	// Compute amount of block groups in FS
	_block_groups = _superblock->blocks_count / _superblock->blocks_per_group;

	if((_superblock->blocks_count % _superblock->blocks_per_group) != 0)
	{
		_block_groups++;
	}
	std::cerr << "block_groups: " << _block_groups << std::endl;

	// Allocate enough memory to keep the table
	_bgd_table = new Ext2FSBlockGroupDescriptor[_block_groups];

	// Since the size of the table is not forced to be a factor of SECTOR_SIZE
	// we need to compute the amount of sectors we should read and then copy
	// one by one the descriptors into the table in the apropriate order.
	unsigned int size_of_bgd_table = sizeof(Ext2FSBlockGroupDescriptor) * _block_groups;
	unsigned int bgd_table_sectors = size_of_bgd_table / SECTOR_SIZE;
	if((size_of_bgd_table % SECTOR_SIZE) != 0)
	{
		bgd_table_sectors++;
	}
	std::cerr << "bgd_table_sectors: " << bgd_table_sectors << std::endl;

	buffer = new unsigned char[bgd_table_sectors * SECTOR_SIZE];
	for(unsigned int i = 0; i < bgd_table_sectors; i++)
	{
		std::cerr << "pentry.start_lba()+4+i: " << (unsigned int) (pentry.start_lba()+4+i) << " buffer+(SECTOR_SIZE*i): " << (unsigned char *) (buffer+(SECTOR_SIZE*i)) << std::endl;
		_hdd.read(pentry.start_lba()+4+i, buffer+(SECTOR_SIZE*i));
	}

	// Copy descriptors into table
	for(unsigned int i = 0; i < _block_groups; i++)
	{
		_bgd_table[i] = *((Ext2FSBlockGroupDescriptor *) (buffer+(i*sizeof(Ext2FSBlockGroupDescriptor))));
	}
	delete[] buffer;
}

unsigned int Ext2FS::blockgroup_for_inode(unsigned int inode)
{
	assert(inode > 0);
	return (inode - 1) / _superblock->inodes_per_group;
}

unsigned int Ext2FS::blockgroup_inode_index(unsigned int inode)
{
	assert(inode > 0);
	return (inode - 1) % _superblock->inodes_per_group;
}

// Free allocated memory
Ext2FS::~Ext2FS()
{
	delete[] _superblock;
	delete[] _bgd_table;
}

struct Ext2FSSuperblock * Ext2FS::superblock()
{
	return _superblock;
}

unsigned int Ext2FS::block_groups()
{
	return _block_groups;
}

struct Ext2FSBlockGroupDescriptor * Ext2FS::block_group(unsigned int index)
{
	assert(index < _block_groups);
	return & _bgd_table[index];
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSInode & inode)
{
	output << "mode: " << inode.mode << std::endl;
	output << "uid: " << inode.uid << std::endl;
	output << "size: " << inode.size << std::endl;
	output << "atime: " << inode.atime << std::endl;
	output << "ctime: " << inode.ctime << std::endl;
	output << "mtime: " << inode.mtime << std::endl;
	output << "dtime: " << inode.dtime << std::endl;
	output << "gid: " << inode.gid << std::endl;
	output << "links_count: " << inode.links_count << std::endl;
	output << "blocks: " << inode.blocks << std::endl;
	output << "flags: " << inode.flags << std::endl;
	//output << "os_dependant_1: " << inode.os_dependant_1 << std::endl;

	//output << std::hex;
	for(unsigned int i = 0; i < 15; i++)
		output << "block[" << i << "]: " << inode.block[i] << std::endl;
	//output << std:dec;

	output << "generation: " << inode.generation << std::endl;
	output << "file_acl: " << inode.file_acl << std::endl;
	output << "directory_acl: " << inode.directory_acl << std::endl;
	output << "faddr: " << inode.faddr << std::endl;
	//output << "os_dependant_2: " << inode.os_dependant_2 << std::endl[3];

	return output;
}

/**
 * TODO: Imprimir las cosas que van en hexa en hexa
 */
std::ostream & operator<<(std::ostream & output, const struct Ext2FSSuperblock & superblock)
{
	output << "inodes_count: " << superblock.inodes_count << std::endl;
	output << "blocks_count: " << superblock.blocks_count << std::endl;
	output << "reserved_blocks_count: " << superblock.reserved_blocks_count << std::endl;
	output << "free_blocks_count: " << superblock.free_blocks_count << std::endl;
	output << "free_inodes_count: " << superblock.free_inodes_count << std::endl;
	output << "first_data_block: " << superblock.first_data_block << std::endl;
	output << "log_block_size: " << superblock.log_block_size << std::endl;
	output << "log_fragment_size: " << superblock.log_fragment_size << std::endl;
	output << "blocks_per_group: " << superblock.blocks_per_group << std::endl;
	output << "fragments_per_group: " << superblock.fragments_per_group << std::endl;
	output << "inodes_per_group: " << superblock.inodes_per_group << std::endl;
	output << "mount_time: " << superblock.mount_time << std::endl;
	output << "write_time: " << superblock.write_time << std::endl;
	output << "mount_count: " << superblock.mount_count << std::endl;
	output << "max_mount_count: " << superblock.max_mount_count << std::endl;
	output << "magic_bytes: " << superblock.magic_bytes << std::endl;
	output << "state: " << superblock.state << std::endl;
	output << "errors: " << superblock.errors << std::endl;
	output << "minor_revision_level: " << superblock.minor_revision_level << std::endl;
	output << "lastcheck: " << superblock.lastcheck << std::endl;
	output << "checkinterval: " << superblock.checkinterval << std::endl;
	output << "creator_os: " << superblock.creator_os << std::endl;
	output << "revision_level: " << superblock.revision_level << std::endl;
	output << "default_reserved_userid: " << superblock.default_reserved_userid << std::endl;
	output << "default_reserved_groupid: " << superblock.default_reserved_groupid << std::endl;
	// -- EXT2_DYNAMIC_REV Specific --
	output << "first_inode: " << superblock.first_inode << std::endl;
	output << "inode_size: " << superblock.inode_size << std::endl;
	output << "block_group_number: " << superblock.block_group_number << std::endl;
	output << "feature_compatibility: " << superblock.feature_compatibility << std::endl;
	output << "feature_incompatibility: " << superblock.feature_incompatibility << std::endl;
	output << "readonly_feature_compatibility: " << superblock.readonly_feature_compatibility << std::endl;
	output << "uuid: ";
	for(int i = 0; i < 16; i++)
		output << superblock.uuid[i];
	output << std::endl;
	output << "volume_name: ";
	for(int i = 0; i < 16; i++)
		output << superblock.volume_name[i];
	output << std::endl;
	output << "last_mounted: ";
	for(int i = 0; i < 64; i++)
		output << superblock.last_mounted[i];
	output << std::endl;
	output << "algo_bitmap: " << superblock.algo_bitmap << std::endl;
	// Performance hints
	output << "prealloc_blocks: " << (unsigned int) superblock.prealloc_blocks << std::endl;
	output << "prealloc_dir_blocks: " << (unsigned int) superblock.prealloc_dir_blocks << std::endl;
	//char alignment[2];
	// Journaling support
	output << "journal_uuid: ";
	for(int i = 0; i < 16; i++)
		output << superblock.journal_uuid[i];
	output << std::endl;
	output << "journal_inode: " << superblock.journal_inode << std::endl;
	output << "journal_device: " << superblock.journal_device << std::endl;
	output << "last_orphan: " << superblock.last_orphan << std::endl;
	// Directory indexing support
	output << "hash_seed: ";
	for(int i = 0; i < 4; i++)
		output << superblock.hash_seed[i];
	output << std::endl;
	output << "default_hash_version: " << (unsigned int) superblock.default_hash_version << std::endl;
	//char padding[3];
	// Other options
	output << "default_mount_options: " << superblock.default_mount_options << std::endl;
	output << "first_meta_bg: " << superblock.first_meta_bg << std::endl;
	//char unused[760];

	return output;
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSBlockGroupDescriptor & bg_descriptor)
{
	output << "block_bitmap: " << bg_descriptor.block_bitmap << std::endl;
	output << "inode_bitmap: " << bg_descriptor.inode_bitmap << std::endl;
	output << "inode_table: " << bg_descriptor.inode_table << std::endl;
	output << "free_blocks_count: " << bg_descriptor.free_blocks_count << std::endl;
	output << "free_inodes_count: " << bg_descriptor.free_inodes_count << std::endl;
	output << "used_dirs_count: " << bg_descriptor.used_dirs_count << std::endl;

	return output;
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSDirEntry & dent)
{
	output << "inode: " << dent.inode << std::endl;
	output << "record_length: " << dent.record_length << std::endl;
	output << "name_length: " << (unsigned short) dent.name_length << std::endl;
	output << "file_type: " << (unsigned short) dent.file_type << std::endl;

	// NULL terminate dent name
	char * dentname = new char[dent.name_length + 1];
	strncpy(dentname, dent.name, dent.name_length);
	dentname[dent.name_length] = '\0';

	output << "name: " << dentname << std::endl;

	delete[] dentname;

	return output;
}

struct Ext2FSInode * Ext2FS::inode_for_path(const char * path)
{
	char * mypath = new char[strlen(path)+1];
	mypath[strlen(path)] = '\0';
	strncpy(mypath, path, strlen(path));

	char * pathtok = strtok(mypath, PATH_DELIM);
	struct Ext2FSInode * inode = NULL;

	while(pathtok != NULL)
	{
		struct Ext2FSInode * prev_inode = inode;
		// std::cerr << "pathtok: " << pathtok << std::endl;
		inode = get_file_inode_from_dir_inode(prev_inode, pathtok);
		pathtok = strtok(NULL, PATH_DELIM);

		delete prev_inode;
	}

	delete[] mypath;
	return inode;
}

unsigned int Ext2FS::blockaddr2sector(unsigned int block)
{
	// Get partition info
	const PartitionEntry & pentry = _hdd[_partition_number];

	// Compute block size by shifting the value 1024
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;

	return pentry.start_lba() + block * sectors_per_block;
}

/**
 * Warning: This method allocates memory that must be freed by the caller
 */
struct Ext2FSInode * Ext2FS::load_inode(unsigned int inode_number)
{
	/* Estructura fisica del file system (http://e2fsprogs.sourceforge.net/ext2intro.html)
	   Ext2 esta compuesto por grupos de bloques. Y la estructura física puede ser representada de la 
	   siguiente forma:
	
		+-------------------------------------------------------+ 
		| Boot		| Block 	| Block 	|  ...	| Block 	|
		| sector 	| group 1	| group 2   |  ... 	| group N 	|
		+-------------------------------------------------------+ 
		
		Cada bloque contiene una copia redundante de información crucial para el control del file system 
		(superbloque y filesystem descriptors) y, además, contiene una parte del filesystem (un bitmap de 
		bloques, un bitma de inodes, una parte de la tabla de inodos y bloques de datos). La estructura de
		un grupo de bloques es:

		+-----------------------------------------------------------+ 
		| Super	| FS 			| Block 	| Inode		| Data 		|
		| Block | descriptors	| Bitmap 	| Bitmap 	| Blocks  	|
		+-----------------------------------------------------------+ 
		
		En nuestro caso, (supongo por lo que estoy leyendo en el codigo :V) tenemos un array de 
		Ext2FSSuperblock en la que la i-esmia entrada representa el superbloque del i-esimo grupo y 
		una array de Ext2FSBlockGroupDescriptor donde cada entrada tiene el resto de la información 
		del grupo.

		Para poder cargar un inodo hay que realizar los siguiente pasos:
		 	1. Encontrar el grupo de bloques que contiene la tabla que marca donde esta el inodo.
		 		Esto lo hacemos con una función que nos dan ellos.
		 	2. Una vez encontrado el grupo, necesitamos conseguir su descritpor
		 		(que tiene toda la información que del grupo que no sea el superbloque.
		 		Esto lo hacemos con otra función que nos dan ellos.
		 	3. Del descriptor sacamos donde esta la tabla de inodos.
		 	4. Nos fijamos el índice del inodo en dicha tabla.
		 	5. Ahora falta conseguir el inodo propiamente dicho. La tabla puede llegar a ocupar más de un
		 		bloque, entonces necesitamos leer el bloque que	tiene guardada la entrada de la tabla que
		 		corresponde a nuestro inodo. Para esto calculamos cuantos inodos entran en un bloque. (El 
		 		tamaño del inodo y el del bloque se encuentra en el superbloque)
		 	6. Ahora que tenemos un bloque de entradas de la tabla, necesitamos calcular el indice del
		 		inodo dentro de este bloque y copiarlo a la ram.

	Using block groups is a big win in terms of reliability: since the control structures are replicated in 
	each block group, it is easy to recover from a filesystem where the superblock has been corrupted.
 	This structure also helps to get good performances: by reducing the distance between 
 	the inode table and the data blocks, it is possible to reduce the disk head seeks during I/O on files.

	In Ext2fs, directories are managed as linked lists of variable length entries. Each entry contains the inode number, the entry length, the file name and its length. By using variable length entries, it is possible to implement long file names without wasting disk space in directories. The structure of a directory entry is shown in this table:
	inode number	entry length	name length	filename
	As an example, The next table represents the structure of a directory containing three files: file1, long_file_name, and f2:
	i1	16	05	file1
	i2	40	14	long_file_name
	i3	12	02	f2
	*/

	/*1*/
	int inodeBlockGroup = blockgroup_for_inode(inode_number);
	/*2*/
	struct Ext2FSBlockGroupDescriptor* blockGroupDescriptor = block_group(inodeBlockGroup);
	/*3*/
	unsigned int inodesTable = blockGroupDescriptor->inode_table;
	/*4*/
	unsigned int inodeIndex = blockgroup_inode_index(inode_number);
	/*5*/
	struct Ext2FSSuperblock * superBlock = superblock();
    int blockSize = 1024 << superBlock->log_block_size;
	unsigned int inodeSize = superBlock->inode_size;
    unsigned int inodesPerBlock = blockSize / inodeSize;
    unsigned int tableBlock = inodeIndex / inodesPerBlock;

    unsigned char* tableEntries = new unsigned char[inodesPerBlock];
    read_block(inodesTable + tableBlock, (unsigned char*) tableEntries);
    
    /*6*/
    unsigned int inodeBlockIndex = inodeIndex % inodesPerBlock;

    struct Ext2FSInode * loadedNode = new struct Ext2FSInode;
   	memcpy((void*) loadedNode, tableEntries + (inodeBlockIndex*inodeSize), sizeof(struct Ext2FSInode));

   	delete[] tableEntries;
   	return loadedNode;

}

unsigned int Ext2FS::get_block_address(struct Ext2FSInode * inode, unsigned int block_number)
{

	int block_size =  1024 << superblock()->log_block_size; // Aca calculo el tamano del bloque (1024^log_block_size bytes)
	int block_pointers = block_size / sizeof(unsigned int);

	if(block_number < 12) return inode->block[block_number]; //Directos, devuelvo la dirección de alguno de los bloques referenciados por el inodo

	unsigned int* blockList = new unsigned int[block_pointers];
	unsigned int blockAddress;

	if(block_number < block_pointers + 12) {  // Simples
		read_block(inode->block[12], (unsigned char*) blockList);
		unsigned int blockIdx1 = block_number - 12;
		blockAddress = blockList[blockIdx1];
	
	} else if(block_number < block_pointers*(block_pointers+1) + 12) { // Dobles
		read_block(inode->block[13], (unsigned char*) blockList); // Obtengo el bloque de direcciones a listas de punteros
		unsigned int blockIdx1 = (block_number - (block_pointers + 12))/block_pointers; // Indice de la lista que corresponde al bloque buscado
		read_block(blockList[blockIdx1], (unsigned char*) blockList); //Obtengo la lista donde esta el puntero al bloque que quiero.
		unsigned int blockIdx2 = block_number - (block_pointers*(blockIdx1 + 1) + 12); //Indice del bloque en dicha lista
		blockAddress = blockList[blockIdx2];

	} else { // Triples
		read_block(inode->block[14], (unsigned char*) blockList); // Obtengo el primer bloque
		unsigned int blockIdx1 = (block_number - (block_pointers*(block_pointers+1) + 12))/(block_pointers*block_pointers*block_pointers); // Indice del puntero a la primer lista
		read_block(inode->block[blockIdx1], (unsigned char*) blockList); // Obtengo el bloque de direcciones a listas de punteros
		unsigned int blockIdx2 = (block_number - (block_pointers*(block_pointers*(blockIdx1 + 1) + 1) + 12))/(block_pointers*block_pointers); // Indice de la lista que corresponde al bloque buscado
		read_block(blockList[blockIdx2], (unsigned char*) blockList); //Obtengo la lista donde esta el puntero al bloque que quiero.
		unsigned int blockIdx3 = block_number - (block_pointers*(blockIdx2 + block_pointers*(blockIdx1 + 1) + 1) + 12); //Indice del bloque en dicha lista
		blockAddress = blockList[blockIdx3];	
	}

	delete blockList;
	return blockAddress;
}

void Ext2FS::read_block(unsigned int block_address, unsigned char * buffer)
{
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;
	for(unsigned int i = 0; i < sectors_per_block; i++)
		_hdd.read(blockaddr2sector(block_address)+i, buffer+i*SECTOR_SIZE);
	}

struct Ext2FSInode * Ext2FS::get_file_inode_from_dir_inode(struct Ext2FSInode * from, const char * filename)
{
	if(from == NULL)
		from = load_inode(EXT2_RDIR_INODE_NUMBER);
	//std::cerr << *from << std::endl;
	assert(INODE_ISDIR(from));

}

fd_t Ext2FS::get_free_fd()
{
	for(fd_t fd = 0; fd < EXT2_MAX_OPEN_FILES; fd++)
	{
		// Warning: This is inefficient
		if(!(_fd_status[fd/sizeof(unsigned int)] & (1 << (fd % sizeof(unsigned int)))))
			return fd;
	}
	return -1;
}

void Ext2FS::mark_fd_as_used(fd_t fd)
{

	_fd_status[fd/sizeof(unsigned int)] = (_fd_status[fd/sizeof(unsigned int)] | (1 << (fd % sizeof(unsigned int))));
}

void Ext2FS::mark_fd_as_free(fd_t fd)
{

	_fd_status[fd/sizeof(unsigned int)] = (_fd_status[fd/sizeof(unsigned int)] ^ (1 << (fd % sizeof(unsigned int))));
}

fd_t Ext2FS::open(const char * path, const char * mode)
{
	fd_t new_fd = get_free_fd();
	if(new_fd < 0)
		return -1;
	mark_fd_as_used(new_fd);

	// We ignore mode
	struct Ext2FSInode * inode = inode_for_path(path);
	assert(inode != NULL);
	std::cerr << *inode << std::endl;

	if(inode == NULL)
		return -1;

	_open_files[new_fd] = *inode;
	delete inode;
	_seek_memory[new_fd] = 0;

	return new_fd;
}

int Ext2FS::read(fd_t filedesc, unsigned char * buffer, int size)
{
	int realsize = ((_seek_memory[filedesc] + size) >= _open_files[filedesc].size)?_open_files[filedesc].size-_seek_memory[filedesc]:size;
	if(realsize > 0)
	{
		int seek = _seek_memory[filedesc];
		unsigned int read = 0;

		// Compute block size by shifting the value 1024
		unsigned int block_size = 1024 << _superblock->log_block_size;
		unsigned int sectors_per_block = block_size / SECTOR_SIZE;

		unsigned int start_block = (seek / block_size);
		unsigned int end_block = ((seek + realsize - 1) / block_size);


		for(unsigned int block = start_block; block <= end_block; block++)
		{
			unsigned int block_address = get_block_address(&_open_files[filedesc], block);

			unsigned char block_buf[block_size];
			read_block(block_address, block_buf);

			do
			{
				buffer[read++] = block_buf[seek++ % block_size];
			} while(((seek % block_size) != 0) && (read < realsize));
		}
	}
	return realsize;
}

int Ext2FS::write(fd_t filedesc, const unsigned char * buffer, int size)
{
	return -1;
}

int Ext2FS::seek(fd_t filedesc, int offset)
{
	//std::cerr << "offset: " << offset << " size: " << _open_files[filedesc].size << std::endl;
	int position = offset;
	if(offset < 0)
		position = _open_files[filedesc].size + offset;
	if(position >= _open_files[filedesc].size)
		position = _open_files[filedesc].size - 1;
	_seek_memory[filedesc] = position;
	return position;
}

int Ext2FS::tell(fd_t filedesc)
{
	return _seek_memory[filedesc];
}

void Ext2FS::close(fd_t filedesc)
{
	mark_fd_as_free(filedesc);
}
