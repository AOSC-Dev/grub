/* of.c - Access the Open Firmware client interface.  */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2003,2004,2005,2007,2008,2009  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <grub/ieee1275/ieee1275.h>
#include <grub/types.h>
#include <grub/misc.h>

#define IEEE1275_PHANDLE_INVALID  ((grub_ieee1275_cell_t) -1)
#define IEEE1275_IHANDLE_INVALID  ((grub_ieee1275_cell_t) 0)
#define IEEE1275_CELL_INVALID     ((grub_ieee1275_cell_t) -1)



int
grub_ieee1275_finddevice (const char *name, grub_ieee1275_phandle_t *phandlep)
{
  struct find_device_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t device;
    grub_ieee1275_cell_t phandle;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "finddevice", 1, 1);
  args.device = (grub_ieee1275_cell_t) name;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  *phandlep = args.phandle;
  if (args.phandle == IEEE1275_PHANDLE_INVALID)
    return -1;
  return 0;
}

int
grub_ieee1275_get_property (grub_ieee1275_phandle_t phandle,
			    const char *property, void *buf,
			    grub_size_t size, grub_ssize_t *actual)
{
  struct get_property_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t phandle;
    grub_ieee1275_cell_t prop;
    grub_ieee1275_cell_t buf;
    grub_ieee1275_cell_t buflen;
    grub_ieee1275_cell_t size;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "getprop", 4, 1);
  args.phandle = phandle;
  args.prop = (grub_ieee1275_cell_t) property;
  args.buf = (grub_ieee1275_cell_t) buf;
  args.buflen = (grub_ieee1275_cell_t) size;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  if (actual)
    *actual = (grub_ssize_t) args.size;
  if (args.size == IEEE1275_CELL_INVALID)
    return -1;
  return 0;
}

int
grub_ieee1275_get_integer_property (grub_ieee1275_phandle_t phandle,
				    const char *property, grub_uint32_t *buf,
				    grub_size_t size, grub_ssize_t *actual)
{
  int ret;
  ret = grub_ieee1275_get_property (phandle, property, (void *) buf, size, actual);
#ifndef GRUB_CPU_WORDS_BIGENDIAN
  /* Integer properties are always in big endian.  */
  if (ret == 0)
    {
      unsigned int i;
      size /= sizeof (grub_uint32_t);
      for (i = 0; i < size; i++)
	buf[i] = grub_be_to_cpu32 (buf[i]);
    }
#endif
  return ret;
}

int
grub_ieee1275_next_property (grub_ieee1275_phandle_t phandle, char *prev_prop,
			     char *prop)
{
  struct get_property_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t phandle;
    grub_ieee1275_cell_t prev_prop;
    grub_ieee1275_cell_t next_prop;
    grub_ieee1275_cell_t flags;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "nextprop", 3, 1);
  args.phandle = phandle;
  args.prev_prop = (grub_ieee1275_cell_t) prev_prop;
  args.next_prop = (grub_ieee1275_cell_t) prop;
  args.flags = (grub_ieee1275_cell_t) -1;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  return (int) args.flags;
}

int
grub_ieee1275_get_property_length (grub_ieee1275_phandle_t phandle,
				   const char *prop, grub_ssize_t *length)
{
  struct get_property_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t phandle;
    grub_ieee1275_cell_t prop;
    grub_ieee1275_cell_t length;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "getproplen", 2, 1);
  args.phandle = phandle;
  args.prop = (grub_ieee1275_cell_t) prop;
  args.length = (grub_ieee1275_cell_t) -1;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  *length = args.length;
  if (args.length == IEEE1275_CELL_INVALID)
    return -1;
  return 0;
}

int
grub_ieee1275_instance_to_package (grub_ieee1275_ihandle_t ihandle,
				   grub_ieee1275_phandle_t *phandlep)
{
  struct instance_to_package_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t ihandle;
    grub_ieee1275_cell_t phandle;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "instance-to-package", 1, 1);
  args.ihandle = ihandle;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  *phandlep = args.phandle;
  if (args.phandle == IEEE1275_PHANDLE_INVALID)
    return -1;
  return 0;
}

int
grub_ieee1275_package_to_path (grub_ieee1275_phandle_t phandle,
			       char *path, grub_size_t len,
			       grub_ssize_t *actual)
{
  struct instance_to_package_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t phandle;
    grub_ieee1275_cell_t buf;
    grub_ieee1275_cell_t buflen;
    grub_ieee1275_cell_t actual;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "package-to-path", 3, 1);
  args.phandle = phandle;
  args.buf = (grub_ieee1275_cell_t) path;
  args.buflen = (grub_ieee1275_cell_t) len;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  if (actual)
    *actual = args.actual;
  if (args.actual == IEEE1275_CELL_INVALID)
    return -1;
  return 0;
}

int
grub_ieee1275_instance_to_path (grub_ieee1275_ihandle_t ihandle,
				char *path, grub_size_t len,
				grub_ssize_t *actual)
{
  struct instance_to_path_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t ihandle;
    grub_ieee1275_cell_t buf;
    grub_ieee1275_cell_t buflen;
    grub_ieee1275_cell_t actual;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "instance-to-path", 3, 1);
  args.ihandle = ihandle;
  args.buf = (grub_ieee1275_cell_t) path;
  args.buflen = (grub_ieee1275_cell_t) len;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  if (actual)
    *actual = args.actual;
  if (args.actual == IEEE1275_CELL_INVALID)
    return -1;
  return 0;
}

int
grub_ieee1275_write (grub_ieee1275_ihandle_t ihandle, const void *buffer,
		     grub_size_t len, grub_ssize_t *actualp)
{
  struct write_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t ihandle;
    grub_ieee1275_cell_t buf;
    grub_ieee1275_cell_t len;
    grub_ieee1275_cell_t actual;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "write", 3, 1);
  args.ihandle = ihandle;
  args.buf = (grub_ieee1275_cell_t) buffer;
  args.len = (grub_ieee1275_cell_t) len;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  if (actualp)
    *actualp = args.actual;
  return 0;
}

int
grub_ieee1275_read (grub_ieee1275_ihandle_t ihandle, void *buffer,
		    grub_size_t len, grub_ssize_t *actualp)
{
  struct write_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t ihandle;
    grub_ieee1275_cell_t buf;
    grub_ieee1275_cell_t len;
    grub_ieee1275_cell_t actual;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "read", 3, 1);
  args.ihandle = ihandle;
  args.buf = (grub_ieee1275_cell_t) buffer;
  args.len = (grub_ieee1275_cell_t) len;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  if (actualp)
    *actualp = args.actual;
  return 0;
}

int
grub_ieee1275_seek (grub_ieee1275_ihandle_t ihandle, grub_disk_addr_t pos,
		    grub_ssize_t *result)
{
  struct write_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t ihandle;
    grub_ieee1275_cell_t pos_hi;
    grub_ieee1275_cell_t pos_lo;
    grub_ieee1275_cell_t result;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "seek", 3, 1);
  args.ihandle = ihandle;
  /* To prevent stupid gcc warning.  */
#if GRUB_IEEE1275_CELL_SIZEOF >= 8
  args.pos_hi = 0;
  args.pos_lo = pos;
#else
  args.pos_hi = (grub_ieee1275_cell_t) (pos >> (8 * GRUB_IEEE1275_CELL_SIZEOF));
  args.pos_lo = (grub_ieee1275_cell_t)
    (pos & ((1ULL << (8 * GRUB_IEEE1275_CELL_SIZEOF)) - 1));
#endif

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;

  if (result)
    *result = args.result;
  return 0;
}

int
grub_ieee1275_peer (grub_ieee1275_phandle_t node,
		    grub_ieee1275_phandle_t *result)
{
  struct peer_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t node;
    grub_ieee1275_cell_t result;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "peer", 1, 1);
  args.node = node;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  *result = args.result;
  if (args.result == 0)
    return -1;
  return 0;
}

int
grub_ieee1275_child (grub_ieee1275_phandle_t node,
		     grub_ieee1275_phandle_t *result)
{
  struct child_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t node;
    grub_ieee1275_cell_t result;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "child", 1, 1);
  args.node = node;
  args.result = IEEE1275_PHANDLE_INVALID;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  *result = args.result;
  if (args.result == 0)
    return -1;
  return 0;
}

int
grub_ieee1275_parent (grub_ieee1275_phandle_t node,
		      grub_ieee1275_phandle_t *result)
{
  struct parent_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t node;
    grub_ieee1275_cell_t result;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "parent", 1, 1);
  args.node = node;
  args.result = IEEE1275_PHANDLE_INVALID;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  *result = args.result;
  return 0;
}

int
grub_ieee1275_interpret (const char *command, grub_ieee1275_cell_t *catch)
{
  struct enter_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t command;
    grub_ieee1275_cell_t catch;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "interpret", 1, 1);
  args.command = (grub_ieee1275_cell_t) command;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  if (catch)
    *catch = args.catch;
  return 0;
}

int
grub_ieee1275_enter (void)
{
  struct enter_args
  {
    struct grub_ieee1275_common_hdr common;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "enter", 0, 0);

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  return 0;
}

void
grub_ieee1275_exit (void)
{
  struct exit_args
  {
    struct grub_ieee1275_common_hdr common;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "exit", 0, 0);

  IEEE1275_CALL_ENTRY_FN (&args);
  for (;;) ;
}

int
grub_ieee1275_open (const char *path, grub_ieee1275_ihandle_t *result)
{
  struct open_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t path;
    grub_ieee1275_cell_t result;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "open", 1, 1);
  args.path = (grub_ieee1275_cell_t) path;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  *result = args.result;
  if (args.result == IEEE1275_IHANDLE_INVALID)
    return -1;
  return 0;
}

int
grub_ieee1275_close (grub_ieee1275_ihandle_t ihandle)
{
  struct close_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t ihandle;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "close", 1, 0);
  args.ihandle = ihandle;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;

  return 0;
}

int
grub_ieee1275_decode_unit4 (grub_ieee1275_ihandle_t ihandle,
                            void *addr, grub_size_t size,
                            grub_uint32_t *phy_lo, grub_uint32_t *phy_hi,
                            grub_uint32_t *lun_lo, grub_uint32_t *lun_hi)
{
  struct decode_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t method;
    grub_ieee1275_cell_t ihandle;
    grub_ieee1275_cell_t size;
    grub_ieee1275_cell_t addr;
    grub_ieee1275_cell_t catch_result;
    grub_ieee1275_cell_t tgt_h;
    grub_ieee1275_cell_t tgt_l;
    grub_ieee1275_cell_t lun_h;
    grub_ieee1275_cell_t lun_l;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "call-method", 4, 5);
  args.method = (grub_ieee1275_cell_t) "decode-unit";
  args.ihandle = ihandle;
  args.size = size;
  args.addr = (grub_ieee1275_cell_t) addr;
  args.catch_result = 1;

  if ((IEEE1275_CALL_ENTRY_FN (&args) == -1) || (args.catch_result))
    {
      grub_error (GRUB_ERR_OUT_OF_RANGE, "decode-unit failed\n");
      return -1;
    }

  *phy_lo = args.tgt_l;
  *phy_hi = args.tgt_h;
  *lun_lo = args.lun_l;
  *lun_hi = args.lun_h;
  return 0;
}

char *
grub_ieee1275_encode_uint4 (grub_ieee1275_ihandle_t ihandle,
                            grub_uint32_t phy_lo, grub_uint32_t phy_hi,
                            grub_uint32_t lun_lo, grub_uint32_t lun_hi,
                            grub_size_t *size)
{
  char *addr;
  struct encode_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t method;
    grub_ieee1275_cell_t ihandle;
    grub_ieee1275_cell_t tgt_h;
    grub_ieee1275_cell_t tgt_l;
    grub_ieee1275_cell_t lun_h;
    grub_ieee1275_cell_t lun_l;
    grub_ieee1275_cell_t catch_result;
    grub_ieee1275_cell_t size;
    grub_ieee1275_cell_t addr;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "call-method", 6, 3);
  args.method = (grub_ieee1275_cell_t) "encode-unit";
  args.ihandle = ihandle;

  args.tgt_l = phy_lo;
  args.tgt_h = phy_hi;
  args.lun_l = lun_lo;
  args.lun_h = lun_hi;
  args.catch_result = 1;

  if ((IEEE1275_CALL_ENTRY_FN (&args) == -1) || (args.catch_result))
    {
      grub_error (GRUB_ERR_OUT_OF_RANGE, "encode-unit failed\n");
      return 0;
    }

  addr = (void *)args.addr;
  *size = args.size;
  addr = grub_strdup ((char *)args.addr);
  return addr;
}

int
grub_ieee1275_claim (grub_addr_t addr, grub_size_t size, unsigned int align,
		     grub_addr_t *result)
{
  struct claim_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t addr;
    grub_ieee1275_cell_t size;
    grub_ieee1275_cell_t align;
    grub_ieee1275_cell_t base;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "claim", 3, 1);
  args.addr = (grub_ieee1275_cell_t) addr;
  args.size = (grub_ieee1275_cell_t) size;
  args.align = (grub_ieee1275_cell_t) align;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  if (result)
    *result = args.base;
  if (args.base == IEEE1275_CELL_INVALID)
    return -1;
  grub_dprintf ("mmap", "CLAIMED: 0x%" PRIxGRUB_IEEE1275_CELL_T " (%"
		PRIuGRUB_IEEE1275_CELL_T " MiB)  size: %" PRIuGRUB_SIZE " MiB\n",
		args.base, args.base >> 20, ALIGN_UP (size, 1 << 20) >> 20);
  return 0;
}

int
grub_ieee1275_release (grub_addr_t addr, grub_size_t size)
{
 struct release_args
 {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t addr;
    grub_ieee1275_cell_t size;
 }
 args;

  INIT_IEEE1275_COMMON (&args.common, "release", 2, 0);
  args.addr = addr;
  args.size = size;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  return 0;
}

int
grub_ieee1275_set_property (grub_ieee1275_phandle_t phandle,
			    const char *propname, const void *buf,
			    grub_size_t size, grub_ssize_t *actual)
{
  struct set_property_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t phandle;
    grub_ieee1275_cell_t propname;
    grub_ieee1275_cell_t buf;
    grub_ieee1275_cell_t size;
    grub_ieee1275_cell_t actual;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "setprop", 4, 1);
  args.size = (grub_ieee1275_cell_t) size;
  args.buf = (grub_ieee1275_cell_t) buf;
  args.propname = (grub_ieee1275_cell_t) propname;
  args.phandle = (grub_ieee1275_cell_t) phandle;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  *actual = args.actual;
  if ((args.actual == IEEE1275_CELL_INVALID) || (args.actual != args.size))
    return -1;
  return 0;
}

int
grub_ieee1275_set_color (grub_ieee1275_ihandle_t ihandle,
			 int index, int r, int g, int b)
{
  struct set_color_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t method;
    grub_ieee1275_cell_t ihandle;
    grub_ieee1275_cell_t index;
    grub_ieee1275_cell_t b;
    grub_ieee1275_cell_t g;
    grub_ieee1275_cell_t r;
    grub_ieee1275_cell_t catch_result;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "call-method", 6, 1);
  args.method = (grub_ieee1275_cell_t) "color!";
  args.ihandle = ihandle;
  args.index = index;
  args.r = r;
  args.g = g;
  args.b = b;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  return args.catch_result;
}

int
grub_ieee1275_milliseconds (grub_uint32_t *msecs)
{
  struct milliseconds_args
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t msecs;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "milliseconds", 0, 1);

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;
  *msecs = args.msecs;
  return 0;
}

int
grub_ieee1275_set_address (grub_ieee1275_ihandle_t ihandle,
                           grub_uint32_t target, grub_uint32_t lun)
{
  struct set_address
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t method;
    grub_ieee1275_cell_t ihandle;
    grub_ieee1275_cell_t tgt;
    grub_ieee1275_cell_t lun;
    grub_ieee1275_cell_t catch_result;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "call-method", 4, 1);

  /*
   * IEEE 1275-1994 Standard for Boot (Initialization Configuration)
   * Firmware: Core Requirements and Practices
   * E.3.2.2 Bus-specific methods for bus nodes
   *
   * A package implementing the scsi-2 device type shall implement the
   * following bus-specific method:
   *
   * set-address ( unit# target# -- )
   * Sets the SCSI target number (0x0..0xf) and unit number (0..7) to which
   * subsequent commands apply.
   */
  args.method = (grub_ieee1275_cell_t) "set-address";
  args.ihandle = ihandle;
  args.tgt = target;
  args.lun = lun;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;

  return args.catch_result;
}

int
grub_ieee1275_no_data_command (grub_ieee1275_ihandle_t ihandle,
                               const void *cmd_addr, grub_ssize_t *result)
{
  struct set_address
  {
    struct grub_ieee1275_common_hdr common;
    grub_ieee1275_cell_t method;
    grub_ieee1275_cell_t ihandle;
    grub_ieee1275_cell_t cmd_addr;
    grub_ieee1275_cell_t error;
    grub_ieee1275_cell_t catch_result;
  }
  args;

  INIT_IEEE1275_COMMON (&args.common, "call-method", 3, 2);

  /*
   * IEEE 1275-1994 Standard for Boot (Initialization Configuration)
   * Firmware: Core Requirements and Practices
   *
   * E.3.2.2 Bus-specific methods for bus nodes
   *
   * A package implementing the scsi-2 device type shall implement the
   * following bus-specific method:
   *
   * no-data-command ( cmd-addr -- error? )
   * Executes a simple SCSI command, automatically retrying under
   * certain conditions.  cmd-addr is the address of a 6-byte command buffer
   * containing an SCSI command that does not have a data transfer phase.
   * Executes the command, retrying indefinitely with the same retry criteria
   * as retry-command.
   *
   * error? is nonzero if an error occurred, zero otherwise.
   * NOTE no-data-command is a convenience function. It provides
   * no capabilities that are not present in retry-command, but for
   * those commands that meet its restrictions, it is easier to use.
   */
  args.method = (grub_ieee1275_cell_t) "no-data-command";
  args.ihandle = ihandle;
  args.cmd_addr = (grub_ieee1275_cell_t) cmd_addr;

  if (IEEE1275_CALL_ENTRY_FN (&args) == -1)
    return -1;

  if (result)
    *result = args.error;

  return args.catch_result;
}

int
grub_ieee1275_get_block_size (grub_ieee1275_ihandle_t ihandle)
{
  struct size_args_ieee1275
    {
      struct grub_ieee1275_common_hdr common;
      grub_ieee1275_cell_t method;
      grub_ieee1275_cell_t ihandle;
      grub_ieee1275_cell_t result;
      grub_ieee1275_cell_t size;
    } args;

  INIT_IEEE1275_COMMON (&args.common, "call-method", 2, 2);
  args.method = (grub_ieee1275_cell_t) "block-size";
  args.ihandle = ihandle;
  args.result = 1;

  if ((IEEE1275_CALL_ENTRY_FN (&args) == -1) || (args.result))
    return 0;

  return args.size;
}
