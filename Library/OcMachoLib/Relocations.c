/** @file
  Provides services for Relocations.

Copyright (c) 2018, Download-Fritz.  All rights reserved.<BR>
This program and the accompanying materials are licensed and made available
under the terms and conditions of the BSD License which accompanies this
distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>

#include <IndustryStandard/AppleMachoImage.h>

#include <Library/DebugLib.h>

/**
  Returns whether the Relocation's type indicates a Pair for the Intel 64
  platform.
  
  @param[in] Type  The Relocation's type to verify.

**/
BOOLEAN
MachoRelocationIsPairIntel64 (
  IN UINT8  Type
  )
{
  return (Type == MachX8664RelocSubtractor);
}

/**
  Returns whether the Relocation's type matches a Pair's for the Intel 64
  platform.
  
  @param[in] Type  The Relocation's type to verify.

**/
BOOLEAN
MachoIsRelocationPairTypeIntel64 (
  IN UINT8  Type
  )
{
  return (Type == MachX8664RelocUnsigned);
}

/**
  Returns whether the Relocation shall be preserved for the Intel 64 platform.
  
  @param[in] Type  The Relocation's type to verify.

**/
BOOLEAN
MachoPreserveRelocationIntel64 (
  IN UINT8  Type
  )
{
  return (Type == MachX8664RelocUnsigned);
}

/**
  Returns whether the Relocation shall be preserved.
  
  @param[in] Type  The Relocation's type to verify.

**/
BOOLEAN
MachoPreserveRelocation (
  IN UINT8  Type
  )
{
  return MachoPreserveRelocationIntel64 (Type);
}

/**
  Retrieves a Relocation by the address it targets.

  @param[in] NumberOfRelocations  Number of Relocations in Relocations.
  @param[in] Relocations          The Relocations to search.
  @param[in] Address              The address to search for.
  @param[in] MachCpuType          CPU Type of the MACH-O.

  @retval NULL  NULL is returned on failure.

**/
CONST MACH_RELOCATION_INFO *
MachoGetRelocationByOffset (
  IN UINTN                       NumberOfRelocations,
  IN CONST MACH_RELOCATION_INFO  *Relocations,
  IN UINT64                      Address,
  IN MACH_CPU_TYPE               MachCpuType
  )
{
  UINTN                      Index;
  CONST MACH_RELOCATION_INFO *Relocation;

  ASSERT (Relocations != NULL);

  for (
    Index = 0, Relocation = &Relocations[0];
    Index < NumberOfRelocations;
    ++Index, ++Relocation
    ) {
    //
    // A section-based relocation entry can be skipped for absolute 
    // symbols.
    //
    if ((((UINT32)Relocation->Address & MACH_RELOC_SCATTERED) == 0)
     && (Relocation->Extern == 0)
     && (Relocation->Address == MACH_RELOC_ABSOLUTE)) {
      continue;
    }

    if ((UINT64)Relocation->Address == Address) {
      return Relocation;
    }
    //
    // Relocation Pairs can be skipped.
    //
    if ((MachCpuType == MachCpuTypeX8664)
     && MachoRelocationIsPairIntel64 (Relocation->Type)) {
      ++Index;
      ++Relocation;
    }
  }

  return NULL;
}
