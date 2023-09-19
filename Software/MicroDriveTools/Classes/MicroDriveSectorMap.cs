using MicroDriveTools.Structs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MicroDriveTools.Classes
{
    public class MicroDriveSectorMap
    {
        public const byte DAMAGED_SECTOR = 0xFF;
        public const byte FREE_SECTOR = 0xFD;
        public const byte MAP_FILE = 0xF8;

        private MicroDriveSectorMapEntry[] entries;
        public MicroDriveSectorMapEntry this[int index] { get { return entries[index]; } }
        public MicroDriveSectorMapEntry[] DamagedSectors { get { return entries.Where(e => e.FileNumber == DAMAGED_SECTOR).ToArray(); } }
        public MicroDriveSectorMapEntry[] FreeSectors { get { return entries.Where(e => e.FileNumber == FREE_SECTOR).ToArray(); } }
        public byte LastAllocatedSector { get; internal set; }
        internal unsafe MicroDriveSectorMap(MicroDriveSector SectorZero)
        {
            if (SectorZero.Header.HeaderFlag != 0xFF || SectorZero.Header.SectorNumber != 0)
                throw new ArgumentException(nameof(SectorZero));

            List<MicroDriveSectorMapEntry> eBuffer = new List<MicroDriveSectorMapEntry>();

            for (int buc = 0; buc < 510; buc += 2)
                eBuffer.Add(new MicroDriveSectorMapEntry { SectorNumber = (byte)(buc / 2), FileNumber = SectorZero.Record.Data[buc], FileBlock = SectorZero.Record.Data[buc + 1] });

            LastAllocatedSector = SectorZero.Record.Data[511];

            entries = eBuffer.ToArray();
        }
        public MicroDriveSectorMap()
        {
            entries = new MicroDriveSectorMapEntry[255];

            for (int buc = 0; buc < 255; buc++)
                entries[buc] = new MicroDriveSectorMapEntry { SectorNumber = (byte)buc, FileNumber = FREE_SECTOR, FileBlock = 0 };

            entries[254].FileNumber = DAMAGED_SECTOR;
            entries[0].FileNumber = MAP_FILE;

            LastAllocatedSector = 255;
        }
        public MicroDriveSectorMapEntry[] GetFileMap(byte FileNumber)
        {
            if (FileNumber >= 0xFD)
                throw new ArgumentException("Invalid FileNumber");

            return entries.Where(e => e.FileNumber == FileNumber).OrderBy(e => e.FileBlock).ToArray();
        }
        public void AssignSector(byte SectorNumber, byte FileNumber, byte FileBlock, bool FlagAsLastAllocated = true)
        {
            entries[SectorNumber] = new MicroDriveSectorMapEntry { SectorNumber = SectorNumber, FileNumber = FileNumber, FileBlock = FileBlock };
            LastAllocatedSector = SectorNumber;
        }
        public void FlagAsEmpty(byte SectorNumber)
        {
            entries[SectorNumber] = new MicroDriveSectorMapEntry { SectorNumber = SectorNumber, FileNumber = FREE_SECTOR, FileBlock = 0 };
        }
        public void FlagAsDamaged(byte SectorNumber)
        {
            entries[SectorNumber] = new MicroDriveSectorMapEntry { SectorNumber = SectorNumber, FileNumber = DAMAGED_SECTOR, FileBlock = 0 };
        }
        public MicroDriveSector ToSectorZero(string MediumName, ushort MediumId)
        {
            MicroDriveSector sector = new MicroDriveSector();
            sector.Record.MakeEmpty();
            sector.Header.HeaderFlag = 0xFF;
            sector.Header.SectorNumber = 0;
            sector.Header.MediumId = MediumId;
            sector.Header.MediumName = MediumName;
            byte[] data = new byte[512];

            for (int buc = 0; buc < 255; buc++)
            {
                int offset = buc * 2;
                data[offset] = entries[buc].FileNumber;
                data[offset + 1] = entries[buc].FileBlock;
            }

            data[510] = 0x01;
            data[511] = LastAllocatedSector;

            sector.Record.FileNumber = MAP_FILE;
            sector.Record.FileBlock = 0;
            sector.Record.ManagedData = data;
            sector.Header.ComputeChecksum();
            sector.Record.ComputeChecksums();

            return sector;
        }

        public MicroDriveSectorMapEntry[] ToArray()
        {
            return entries.Select(e => new MicroDriveSectorMapEntry { SectorNumber = e.SectorNumber, FileBlock = e.FileBlock, FileNumber = e.FileNumber }).ToArray();
        }
    }
}
