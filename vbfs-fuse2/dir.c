#include "utils.h"
#include "log.h"
#include "mempool.h"
#include "vbfs-fuse.h"
#include "inode.h"

static void load_dentry_info(vbfs_dir_meta_dk_t *dir_meta_disk,
				struct dentry_info *dir_info)
{
	dir_info->group_no = dir_meta_disk->vbfs_dir_meta.group_no;
	dir_info->total_extends = dir_meta_disk->vbfs_dir_meta.total_extends;

	dir_info->dir_self_count = dir_meta_disk->vbfs_dir_meta.dir_self_count;
	dir_info->dir_total_count = dir_meta_disk->vbfs_dir_meta.dir_total_count;

	dir_info->next_extend = dir_meta_disk->vbfs_dir_meta.next_extend;
	dir_info->dir_capacity = dir_meta_disk->vbfs_dir_meta.dir_capacity;
	dir_info->bitmap_size = dir_meta_disk->vbfs_dir_meta.bitmap_size;
}

static void save_dentry_info(vbfs_dir_meta_dk_t *dir_meta_disk,
				struct dentry_info *dir_info)
{
	memset(dir_meta_disk, 0, sizeof(vbfs_dir_meta_dk_t));

	dir_meta_disk->vbfs_dir_meta.group_no = dir_info->group_no;
	dir_meta_disk->vbfs_dir_meta.total_extends = dir_info->total_extends;

	dir_meta_disk->vbfs_dir_meta.dir_self_count = dir_info->dir_self_count;
	dir_meta_disk->vbfs_dir_meta.dir_total_count = dir_info->dir_total_count;

	dir_meta_disk->vbfs_dir_meta.next_extend = dir_info->next_extend;
	dir_meta_disk->vbfs_dir_meta.dir_capacity = dir_info->dir_capacity;
	dir_meta_disk->vbfs_dir_meta.bitmap_size = dir_info->bitmap_size;
}

static void load_dirent(struct vbfs_dirent_disk * dir_dk, struct dentry_vbfs *dir)
{
	dir->inode = cpu_to_le32(dir_dk->inode);
	dir->file_type = cpu_to_le32(dir_dk->file_type);

	memcpy(dir->name, dir_dk->name, NAME_LEN);
}

static void save_dirent(struct vbfs_dirent_disk * dir_dk, struct dentry_vbfs *dir)
{
	dir_dk->inode = le32_to_cpu(dir->inode);
	dir_dk->file_type = le32_to_cpu(dir->file_type);

	memcpy(dir_dk->name, dir->name, NAME_LEN);
}

static int get_dentry_in_extend(struct dentry_info *dir_info,
				char *extend, struct list_head *dir_list)
{
	int ret = 0, i = 0;
	__u32 offset = 0;
	char *bitmap = NULL;
	__u32 bitmap_bits = 0;
	struct dentry_vbfs *dir = NULL;
	char *pos = NULL;

	bitmap_bits = dir_info->bitmap_size * 4096;

	for (i = 0; i < dir_info->dir_self_count; i ++) {
		offset = bitops_next_pos(bitmap, bitmap_bits, offset);
		if (offset == 0) {
			log_err("BUG\n");
			break;
		}
		dir = mp_malloc(sizeof(struct dentry_vbfs));

		pos = extend + (offset - 1) * VBFS_DIR_SIZE;
		load_dirent((struct vbfs_dirent_disk *) pos, dir);

		list_add(&dir->dentry_list, dir_list);
	}

	return ret;
}

int get_dentry(struct inode_vbfs *inode_v, struct list_head *dir_list)
{
	struct dentry_info fst_dir_info;
	int ret = 0;

	pthread_mutex_lock(&inode_v->lock_inode);

	if (! (inode_v->i_mode | VBFS_FT_DIR)) {
		pthread_mutex_unlock(&inode_v->lock_inode);
		return -ENOTDIR;
	}

	ret = inode_get_first_extend_unlocked(inode_v);
	if (ret) {
		pthread_mutex_unlock(&inode_v->lock_inode);
		return ret;
	}

	load_dentry_info((vbfs_dir_meta_dk_t *) inode_v->inode_first_ext, &fst_dir_info);

	ret = get_dentry_in_extend(&fst_dir_info, inode_v->inode_first_ext, dir_list);
	if (ret) {
		pthread_mutex_unlock(&inode_v->lock_inode);
		return ret;
	}

	if (fst_dir_info.total_extends > 1) {
		/* Not Implement that dir use more than one extend */
		log_err("Not Implement\n");
	}

	pthread_mutex_unlock(&inode_v->lock_inode);

	return 0;
}

int put_dentry(struct list_head *dir_list)
{
	struct dentry_vbfs *dentry = NULL;
	struct dentry_vbfs *tmp = NULL;

	list_for_each_entry_safe(dentry, tmp, dir_list, dentry_list) {
		list_del(&dentry->dentry_list);
		mp_free(dentry, sizeof(struct dentry_vbfs));
	}

	INIT_LIST_HEAD(dir_list);

	return 0;
}

int add_dentry()
{
	return 0;
}

int vbfs_readdir(struct inode_vbfs *inode_v, off_t *filler_pos, fuse_fill_dir_t filler, void *filler_buf)
{
	return 0;
}
