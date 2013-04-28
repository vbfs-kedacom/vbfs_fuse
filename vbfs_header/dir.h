#ifndef __DIR_H_ 
#define __DIR_H_

/* vbfs_mkdir */
int vbfs_mkdir(struct inode_vbfs *i_vbfs_parent, const char *dirname);

/* vbfs_readdir */
int vbfs_readdir(struct inode_vbfs *dir_v_inode, off_t *filler_pos, fuse_fill_dir_t filler, void *filler_buf);

/* vbfs_rmdir */
int vbfs_rmdir(struct inode_vbfs *dir_v_inode, const char *dirname);

/* vbfs_dir_is_empty */
int vbfs_dir_is_empty(struct inode_vbfs *dir_v_inode);

#endif
