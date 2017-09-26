=====================================================
		  Eidis/GameSX Presents
                   Engrish version of
                 SxSI v5 with DMA patch 
=====================================================

 This archive contains SCSI compatible IOCS driver
for X68000 machines with SASI port and features DMA
patch which prevents data corruption while writing
data to disk.

 In this floppy image you will find translated
versions of the following programs:

HUMAN.SYS    - Human OS hardware interfacing routines
	       and the DOS initialization program.
COMMAND.X    - Default operating system shell. 
ED.X         - Text editor
SCSIFORMAT.X - SCSI UNIT initialization program
BOOTSET.X    - SCSI device boot program for the
	       SASI port (Installs to SRAM)
SASIIOCS.SYS - SCSI port for SCSI-IOCS
SCSIHD.SYS   - SCSI Block Device driver.


Foreword
=====================================================

 Early X68000 models such as ACE, EXPERT, PRO,
EXPERT II and PRO II can interface with hard drives
through the built in SASI port which is a fully
compliant subset of SCSI-1. This means that
electronically SASI and SCSI-1 are compatible and
that the only difference is in the software which
drives them. The largest known SASI hard drive has 40MB
capacity which even during the X68000 glory days was
too small and especially too loud.
 The SxSI driver on the other hand allows interfacing
with 8bit bus wide SCSI-1 devices and is limited to
the maximum size of 1GB. The SCSI-1 hard drives are
very slow, loud and given their age can not be
considered as a reliable storage media.
 For many years the workaround for this problem was
to use a SCSI to IDE adapter and connect an IDE hard
drive or CF reader to it.

Some compatible SCSI to IDE adapters are:

* I-O Data IDSC21-E
* Yamaha V769970
* Acard AEC-7720U

 They all have been surpassed by STRATOS CF AztecMonster
which has a built in CF reader and is fully compatible
with X68000. Sometimes it costs even less than any of
the previous mentioned adapters.


How to format, partition hard disk and make it boot
=====================================================

 Set your device to SCSI ID0 and boot from the
MasterDiskV2. Upon booting you will be greated with
a DOS prompt.

Now type the following commands:

cd sxsi
scsiformat

*Press 0 to select device with ID0. The program will
 ask you to initialize and format the hard disk.
 Answer Y to both questions.
*Press 2 to create new partition.
*Press A if you want to allocate all available space
 or type in the amout of MB you wish to allocate for
 the new partition.
*Enter 32 for maximum root directory entries. This
 means that you will be able to go 32 directories
 deep from the root directory.
*Press Y to transfer HUMAN.SYS to created partition.
*Press A to select A: as the source drive.
*Press 4 to enter Bootflags menu.
*Press 1 to select 1st partition.
*Press 0 to make it bootable.
*Press 8 to exit the sxsiformat program.
*Reboot the X68000 from MasterDiskV2

cs sxsi
bootset

*Enter ED0100 as destination SRAM address.
*Enter 1 as boot delay in seconds.

cd ..
copy command.x c:

Now remove floppy disk, reboot and ...

Keep the scene alive !
Eidis 