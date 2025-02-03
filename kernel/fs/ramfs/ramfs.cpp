#include "fs/ramfs.hpp"
folder_inode *ramfs::get_root_directory() { return &__root_dir; }
void ramfs::dlfilenode(file_inode* fd)
{
    fd->prune_refs();
    __file_nodes.erase(*fd);
    this->syncdirs();
}
void ramfs::dldirnode(folder_inode* dd)
{
    if(!dd->is_empty()) { throw std::logic_error{ std::string{ "cannot delete folder " } + dd->name() + " because it is not empty" }; }
    dd->prune_refs();
    __folder_nodes.erase(*dd);
    this->syncdirs();
}
file_inode *ramfs::mkfilenode(folder_inode *parent, std::string const &name) { return std::addressof(*(__file_nodes.emplace(name, this->next_fd++).first)); }
folder_inode *ramfs::mkdirnode(folder_inode *parent, std::string const &name) { return std::addressof(*(__folder_nodes.emplace(name).first)); }
void ramfs::syncdirs() { for(ramfs_folder_inode& folder : __folder_nodes) folder.fsync(); }
dev_t ramfs::xgdevid() const noexcept { return ramfs_magic; }