/*
 * Copyright (C) 2003-2008 Takahiro Hirofuchi
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 */

#ifndef __USBIP_COMMON_H
#define __USBIP_COMMON_H

#include <linux/compiler.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/net.h>
#include <linux/printk.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/usb.h>
#include <linux/wait.h>

#define USBIP_VERSION USBIP_VERSION_STRING

#undef pr_fmt

#ifdef DEBUG
#define pr_fmt(fmt)     KBUILD_MODNAME ": %s:%d: " fmt, __func__, __LINE__
#else
#define pr_fmt(fmt)     KBUILD_MODNAME ": " fmt
#endif

enum {
	usbip_debug_xmit	= (1 << 0),
	usbip_debug_sysfs	= (1 << 1),
	usbip_debug_urb		= (1 << 2),
	usbip_debug_eh		= (1 << 3),

	usbip_debug_stub_cmp	= (1 << 8),
	usbip_debug_stub_dev	= (1 << 9),
	usbip_debug_stub_rx	= (1 << 10),
	usbip_debug_stub_tx	= (1 << 11),

	usbip_debug_vhci_rh	= (1 << 8),
	usbip_debug_vhci_hc	= (1 << 9),
	usbip_debug_vhci_rx	= (1 << 10),
	usbip_debug_vhci_tx	= (1 << 11),
	usbip_debug_vhci_sysfs  = (1 << 12)
};

#define usbip_dbg_flag_xmit	(usbip_debug_flag & usbip_debug_xmit)
#define usbip_dbg_flag_vhci_rh	(usbip_debug_flag & usbip_debug_vhci_rh)
#define usbip_dbg_flag_vhci_hc	(usbip_debug_flag & usbip_debug_vhci_hc)
#define usbip_dbg_flag_vhci_rx	(usbip_debug_flag & usbip_debug_vhci_rx)
#define usbip_dbg_flag_vhci_tx	(usbip_debug_flag & usbip_debug_vhci_tx)
#define usbip_dbg_flag_stub_rx	(usbip_debug_flag & usbip_debug_stub_rx)
#define usbip_dbg_flag_stub_tx	(usbip_debug_flag & usbip_debug_stub_tx)
#define usbip_dbg_flag_vhci_sysfs  (usbip_debug_flag & usbip_debug_vhci_sysfs)

extern unsigned long usbip_debug_flag;
extern struct device_attribute dev_attr_usbip_debug;

#define usbip_dbg_with_flag(flag, fmt, args...)		\
	do {						\
		if (flag & usbip_debug_flag)		\
			pr_debug(fmt, ##args);		\
	} while (0)

#define usbip_dbg_sysfs(fmt, args...) \
	usbip_dbg_with_flag(usbip_debug_sysfs, fmt , ##args)
#define usbip_dbg_xmit(fmt, args...) \
	usbip_dbg_with_flag(usbip_debug_xmit, fmt , ##args)
#define usbip_dbg_urb(fmt, args...) \
	usbip_dbg_with_flag(usbip_debug_urb, fmt , ##args)
#define usbip_dbg_eh(fmt, args...) \
	usbip_dbg_with_flag(usbip_debug_eh, fmt , ##args)

#define usbip_dbg_vhci_rh(fmt, args...)	\
	usbip_dbg_with_flag(usbip_debug_vhci_rh, fmt , ##args)
#define usbip_dbg_vhci_hc(fmt, args...)	\
	usbip_dbg_with_flag(usbip_debug_vhci_hc, fmt , ##args)
#define usbip_dbg_vhci_rx(fmt, args...)	\
	usbip_dbg_with_flag(usbip_debug_vhci_rx, fmt , ##args)
#define usbip_dbg_vhci_tx(fmt, args...)	\
	usbip_dbg_with_flag(usbip_debug_vhci_tx, fmt , ##args)
#define usbip_dbg_vhci_sysfs(fmt, args...) \
	usbip_dbg_with_flag(usbip_debug_vhci_sysfs, fmt , ##args)

#define usbip_dbg_stub_cmp(fmt, args...) \
	usbip_dbg_with_flag(usbip_debug_stub_cmp, fmt , ##args)
#define usbip_dbg_stub_rx(fmt, args...) \
	usbip_dbg_with_flag(usbip_debug_stub_rx, fmt , ##args)
#define usbip_dbg_stub_tx(fmt, args...) \
	usbip_dbg_with_flag(usbip_debug_stub_tx, fmt , ##args)

#include "usbip_struct.h"

enum usbip_side {
	USBIP_VHCI,
	USBIP_STUB,
};

enum usbip_status {
	/* sdev is available. */
	SDEV_ST_AVAILABLE = 0x01,
	/* sdev is now used. */
	SDEV_ST_USED,
	/* sdev is unusable because of a fatal error. */
	SDEV_ST_ERROR,

	/* vdev does not connect a remote device. */
	VDEV_ST_NULL,
	/* vdev is used, but the USB address is not assigned yet */
	VDEV_ST_NOTASSIGNED,
	VDEV_ST_USED,
	VDEV_ST_ERROR
};

/* event handler */
#define USBIP_EH_SHUTDOWN	(1 << 0)
#define USBIP_EH_BYE		(1 << 1)
#define USBIP_EH_RESET		(1 << 2)
#define USBIP_EH_UNUSABLE	(1 << 3)

#define SDEV_EVENT_REMOVED   (USBIP_EH_SHUTDOWN | USBIP_EH_RESET | USBIP_EH_BYE)
#define	SDEV_EVENT_DOWN		(USBIP_EH_SHUTDOWN | USBIP_EH_RESET)
#define	SDEV_EVENT_ERROR_TCP	(USBIP_EH_SHUTDOWN | USBIP_EH_RESET)
#define	SDEV_EVENT_ERROR_SUBMIT	(USBIP_EH_SHUTDOWN | USBIP_EH_RESET)
#define	SDEV_EVENT_ERROR_MALLOC	(USBIP_EH_SHUTDOWN | USBIP_EH_UNUSABLE)

#define	VDEV_EVENT_REMOVED	(USBIP_EH_SHUTDOWN | USBIP_EH_BYE)
#define	VDEV_EVENT_DOWN		(USBIP_EH_SHUTDOWN | USBIP_EH_RESET)
#define	VDEV_EVENT_ERROR_TCP	(USBIP_EH_SHUTDOWN | USBIP_EH_RESET)
#define	VDEV_EVENT_ERROR_MALLOC	(USBIP_EH_SHUTDOWN | USBIP_EH_UNUSABLE)

struct usbip_device;
struct usbip_filter;

struct usbip_filter_driver {
	struct list_head list;
    char *name;
    void *(*probe)(struct usbip_device *ud, 
            struct usb_interface *interface);
    void (*remove)(struct usbip_filter *filter);
    int (*on_tx)(struct usbip_filter *filter, struct urb *urb);
    int (*on_rx)(struct usbip_filter *filter, 
            struct usbip_header *pdu, struct urb *urb);
};

void usbip_filter_register(struct usbip_filter_driver *drv);
void usbip_filter_unregister(struct usbip_filter_driver *drv);
void usbip_filter_probe(struct usbip_device *ud,
        struct usb_interface *interface);
void usbip_filter_remove(struct usbip_device *ud,
        struct usbip_filter_driver *drv);
int usbip_filter_on_rx(struct usbip_device *ud, 
        struct usbip_header *pdu, struct urb *urb);
int usbip_filter_on_tx(struct usbip_device *ud, struct urb *urb);

struct usbip_filter {
	struct list_head list;
    struct usbip_device *ud;
    struct usbip_filter_driver *drv;
    void *priv;
};

/* a common structure for stub_device and vhci_device */
struct usbip_device {
	enum usbip_side side;
	enum usbip_status status;

	/* lock for status */
	spinlock_t lock;

	struct socket *tcp_socket;

	struct task_struct *tcp_rx;
	struct task_struct *tcp_tx;

	unsigned long event;
	struct task_struct *eh;
	wait_queue_head_t eh_waitq;

	struct eh_ops {
		void (*shutdown)(struct usbip_device *);
		void (*reset)(struct usbip_device *);
		void (*unusable)(struct usbip_device *);
	} eh_ops;

	spinlock_t filter_lock;
	struct list_head filters;
};

#define kthread_get_run(threadfn, data, namefmt, ...)			   \
({									   \
	struct task_struct *__k						   \
		= kthread_create(threadfn, data, namefmt, ## __VA_ARGS__); \
	if (!IS_ERR(__k)) {						   \
		get_task_struct(__k);					   \
		wake_up_process(__k);					   \
	}								   \
	__k;								   \
})

#define kthread_stop_put(k)		\
	do {				\
		kthread_stop(k);	\
		put_task_struct(k);	\
	} while (0)

/* usbip_common.c */
void usbip_dump_urb(struct urb *purb);
void usbip_dump_header(struct usbip_header *pdu);

int usbip_recv(struct socket *sock, void *buf, int size);
struct socket *sockfd_to_socket(unsigned int sockfd);

void usbip_pack_pdu(struct usbip_header *pdu, struct urb *urb, int cmd,
		    int pack);
void usbip_header_correct_endian(struct usbip_header *pdu, int send);

struct usbip_iso_packet_descriptor*
usbip_alloc_iso_desc_pdu(struct urb *urb, ssize_t *bufflen);

/* some members of urb must be substituted before. */
int usbip_recv_iso(struct usbip_device *ud, struct urb *urb);
void usbip_pad_iso(struct usbip_device *ud, struct urb *urb);
int usbip_recv_xbuff(struct usbip_device *ud, struct urb *urb);

/* usbip_event.c */
int usbip_start_eh(struct usbip_device *ud);
void usbip_stop_eh(struct usbip_device *ud);
void usbip_event_add(struct usbip_device *ud, unsigned long event);
int usbip_event_happened(struct usbip_device *ud);

static inline int interface_to_busnum(struct usb_interface *interface)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	return udev->bus->busnum;
}

static inline int interface_to_devnum(struct usb_interface *interface)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	return udev->devnum;
}

#endif /* __USBIP_COMMON_H */
