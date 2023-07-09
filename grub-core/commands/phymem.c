/* phymem.c - Get the amount of system memory and save it to $total_mem */
/* It traverses the entire grub memory map to get the total amount. */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2003,2007  Free Software Foundation, Inc.
 *  Copyright (C) 2003  NIIBE Yutaka <gniibe@m17n.org>
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

#include <grub/types.h>
#include <grub/misc.h>
#include <grub/memory.h>
#include <grub/mm.h>
#include <grub/err.h>
#include <grub/dl.h>
#include <grub/extcmd.h>
#include <grub/i18n.h>
#include <grub/env.h>

GRUB_MOD_LICENSE ("GPLv3+");

// Round to multiple of specific value
#ifndef GRUB_MACHINE_EFI
// 4 MiB
#define ROUND_BITS 22
#else
// 128 MiB
#define ROUND_BITS 27
#endif

/* Traverse the entire GRUB memory map, add up valid sections.
 * The result is total amount of system memory.
 */
static int
traverse_mmap_hook (grub_uint64_t addr __attribute__ ((unused)), grub_uint64_t size, grub_memory_type_t type,
                    void *data)
{
  // Skip the reserved sections.
  if (type != GRUB_MEMORY_RESERVED) {
    unsigned long long *total = (unsigned long long *) data;
    *total += size;
  }
  return 0;
}

/* The actual command.
 * It calls grub_machine_mmap_iterate(), get the result, and round it to a multiple of 2MiB or 128MiB.
 * The results in MiB are exported to $total_mem and $total_mem_rounded.
 */
static grub_err_t
grub_cmd_phymem (grub_extcmd_context_t ctxt __attribute__ ((unused)),
                int argc __attribute__ ((unused)),
                char **args __attribute__ ((unused)))
{
  unsigned long long total_mem_bytes = 0ULL;
  unsigned long long total_mem_bytes_rounded = 0ULL;
#ifndef GRUB_MACHINE_EMU
  // Prevent it from being accumulated everytime the command being invoked.
  if (total_mem_bytes == 0) {
    // Calculate the amount of memory.
    grub_err_t err = grub_machine_mmap_iterate (traverse_mmap_hook, &total_mem_bytes);
    if (err) {
      return err;
    }
  }

  // On some platforms (especially i386-pc), the result is usually not the right amount.
  // We round it up for a bit, to get a "real" amount.
  total_mem_bytes_rounded = (total_mem_bytes + (1 << ROUND_BITS) - 1ULL) >> ROUND_BITS;
  total_mem_bytes_rounded = total_mem_bytes_rounded << ROUND_BITS;
#endif /* GRUB_MACHINE_EMU */

  char *total_mem_mebibytes_str = grub_xasprintf ("%llu", total_mem_bytes / 0x100000);
  char *total_mem_mebibytes_rounded_str = grub_xasprintf ("%llu", total_mem_bytes_rounded / 0x100000);

  if (!total_mem_mebibytes_str || !total_mem_mebibytes_rounded_str) {
    return grub_error (GRUB_ERR_OUT_OF_MEMORY, "Out of memory while allocating strings");
  }

  grub_printf_ (N_("The total system memory is %s MiB (%llu Bytes).\n"), total_mem_mebibytes_str, total_mem_bytes);

  if (total_mem_bytes != total_mem_bytes_rounded) {
    grub_printf_ (N_("Rounded to %s MiB (%llu Bytes).\n"), total_mem_mebibytes_rounded_str, total_mem_bytes_rounded);
  }

  // Export them to the environment variables.
  grub_env_set ("total_mem", total_mem_mebibytes_str);
  grub_env_export ("total_mem");
  grub_env_set ("total_mem_rounded", total_mem_mebibytes_rounded_str);
  grub_env_export ("total_mem_rounded");

  return GRUB_ERR_NONE;
}

static grub_extcmd_t cmd;

GRUB_MOD_INIT(phymem)
{
  cmd = grub_register_extcmd ("phymem", grub_cmd_phymem, 0, 0,
                              N_("Get total amount of physical memory in MiB."), 0);
}

GRUB_MOD_FINI(phymem)
{
  grub_unregister_extcmd (cmd);
}
