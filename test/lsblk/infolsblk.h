#ifndef INFOLSBLK_H
#define INFOLSBLK_H

#include "../../utils/json/cxxJson.h"

struct TInfoLsblkItem
{
    std::string 	name	;    //	设备名
    std::string 	kname	;    //	内部内核设备名称
    std::string 	path	;    //	path to the device node
    std::string 	maj_min	;    //	主:次 设备号 maj:min
    unsigned long long 	fsavail	;    //	文件系统可用大小
    unsigned long long 	fssize	;    //	文件系统大小
    std::string 	fstype	;    //	文件系统类型
    unsigned long long 	fsused	;    //	文件系统已用大小
    std::string 	fsuse_	;    //	文件系统使用百分比 fsuse%
    std::string 	mountpoint	;    //	设备挂载位置
    std::string 	label	;    //	文件系统标签
    std::string 	uuid	;    //	文件系统 UUID
    std::string 	ptuuid	;    //	partition table identifier (usually UUID)
    std::string 	pttype	;    //	partition table type
    std::string 	parttype	;    //	分区类型 UUID
    std::string 	partlabel	;    //	分区 LABEL
    std::string 	partuuid	;    //	分区 UUID
    std::string 	partflags	;    //	分区标志
    long long       ra	;    //	该设备的预读(read_ahead)
    bool            ro	;    //	只读设备
    bool            rm	;    //	可移动设备
    bool            hotplug	;    //	可移动或热插拔设备(usb, pcmcia, ...)
    std::string 	model	;    //	设备标识符
    std::string 	serial	;    //	磁盘序列号
    std::string 	size	;    //	磁盘容量
    std::string 	state	;    //	设备的状态
    std::string 	owner	;    //	用户名
    std::string 	group	;    //	组名
    std::string 	mode	;    //	设备节点权限
    int         alignment	;    //	对齐偏移量
    int             min_io	;    //	最小 I/O 大小 min-io
    int             opt_io	;    //	最优 I/O 大小 opt-io
    int             phy_sec	;    //	物理扇区大小 phy-sec
    int             log_sec	;    //	逻辑扇区大小 log-sec
    bool            rota	;    //	转动设备
    std::string 	sched	;    //	I/O 调度器名称
    int             rq_size	;    //	请求队列大小 rq-size
    std::string 	type	;    //	设备类型
    int         disc_aln	;    //	忽略对齐偏移量 disc-aln
    std::string 	disc_gran	;    //	忽略粒度 disc-gran
    std::string 	disc_max	;    //	忽略最大字节数 disc-max
    bool        disc_zero	;    //	忽略零数据 disc-zero
    std::string 	wsame	;    //	写相同的最大字节数
    std::string 	wwn	;    //	惟一存储标识符
    bool 	rand	;    //	添加随机性
    std::string 	pkname	;    //	内部上级内核设备名称
    std::string 	hctl	;    //	SCSI 的 Host:Channel:Target:Lun
    std::string 	tran	;    //	设备传输类型
    std::string 	subsystems	;    //	对成链的子系统去重
    std::string 	rev	;    //	设备修订版本
    std::string 	vendor	;    //	设备制造商
    std::string 	zoned	;    //	zone model

    std::vector<TInfoLsblkItem> children;

    CJS_JSON_SERIALIZE(TInfoLsblkItem, name, kname, path, maj_min, fsavail,
                       fssize, fstype, fsused, fsuse_, mountpoint,
                       label, uuid, ptuuid, pttype, parttype,
                       partlabel, partuuid, partflags, ra, ro,
                       rm, hotplug, model, serial, size,
                       state, owner, group, mode, alignment,
                       min_io, opt_io, phy_sec, log_sec, rota,
                       sched, rq_size, type, disc_aln, disc_gran,
                       disc_max, disc_zero, wsame, wwn, rand,
                       pkname, hctl, tran, subsystems, rev,
                       vendor, zoned, children)
};

struct TInfoLsblk
{
    std::vector<TInfoLsblkItem> blockdevices;

    CJS_JSON_SERIALIZE(TInfoLsblk, blockdevices)
};

#endif // INFOLSBLK_H
