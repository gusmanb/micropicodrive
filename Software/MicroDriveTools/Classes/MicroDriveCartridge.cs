using MicroDriveTools.Structs;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MicroDriveTools.Classes
{
    public class MicroDriveCartridge
    {
        const int MAX_SECTORS = 255;

        const int MDV_CARTRIDGE_SIZE = 174930;
        const int MDV_PREAMBLE_SIZE = 12;
        const int MDV_HEADER_SIZE = 16;
        const int MDV_DATA_SIZE = 646;
        const int MDV_PAD_SIZE = 34;
        const int MDV_SECTOR_SIZE = 686;

        const int MPD_CARTRIDGE_SIZE = 160140;

        public unsafe static MicroDriveCartridge LoadMDV(string CartridgeFile)
        {
            if(string.IsNullOrWhiteSpace(CartridgeFile))
                throw new ArgumentNullException(nameof(CartridgeFile));

            if (!File.Exists(CartridgeFile))
                throw new FileNotFoundException("Cartridge file not found");

            var rawData = File.ReadAllBytes(CartridgeFile);

            if (rawData.Length != MDV_CARTRIDGE_SIZE)
                throw new ArgumentException("Invalid MDV cartridge");

            MicroDriveSector[] sectors = new MicroDriveSector[255];

            fixed(byte* ptr = rawData) 
            {
                int filePos = MDV_CARTRIDGE_SIZE - MDV_SECTOR_SIZE;

                List<MicroDriveSector> sectorBuffer = new List<MicroDriveSector>();

                for (int buc = 0; buc < 255; buc++)
                {
                    filePos = buc * MDV_SECTOR_SIZE + MDV_PREAMBLE_SIZE; //skip preamble
                    MicroDriveSector sector = new MicroDriveSector();

                    MicroDriveHeader* bufferHeader = (MicroDriveHeader*)(ptr + filePos);
                    sector.Header = *bufferHeader;

                    filePos += MDV_HEADER_SIZE + MDV_PREAMBLE_SIZE;

                    MicroDriveRecord* record = (MicroDriveRecord*)(ptr + filePos);
                    sector.Record= *record;

                    sectorBuffer.Add(sector);
                }

                sectors = sectorBuffer.ToArray();
            }

            return new MicroDriveCartridge(sectors);
        }

        public unsafe static MicroDriveCartridge LoadMPD(string CartridgeFile)
        {
            if (string.IsNullOrWhiteSpace(CartridgeFile))
                throw new ArgumentNullException(nameof(CartridgeFile));

            if (!File.Exists(CartridgeFile))
                throw new FileNotFoundException("Cartridge file not found");

            var rawData = File.ReadAllBytes(CartridgeFile);

            if (rawData.Length != MPD_CARTRIDGE_SIZE)
                throw new ArgumentException("Invalid MPD cartridge");

            MicroDriveSector[] sectors = new MicroDriveSector[255];

            fixed (byte* ptr = rawData)
            {
                MicroDriveSector* secPtr = (MicroDriveSector*)ptr;

                for (int buc = 0; buc < 255; buc++)
                    sectors[buc] = secPtr[buc];
            }

            return new MicroDriveCartridge(sectors);
        }

        public MicroDriveSector[] Sectors { get; private set; }

        public MicroDriveSectorMap Map { get; private set; }

        public MicroDriveDirectory Directory { get; private set; }

        public string MediumName { get { return Sectors.Where(s => s.Header.HeaderFlag == 0xFF && s.Header.SectorNumber == 0).First().Header.MediumName; } }

        internal MicroDriveCartridge(MicroDriveSector[] Sectors, bool FixChecksums = true) 
        {
            if(Sectors == null || Sectors.Length != 255) 
                throw new ArgumentNullException(nameof(Sectors));

            this.Sectors = Sectors;

            if (FixChecksums)
            {
                for(int buc = 0; buc < 255; buc++)
                    FixSectorChecksums(ref this.Sectors[buc]);
            }

            var sectorsZero = this.Sectors.Where(s => s.Header.HeaderFlag == 0xFF && s.Header.SectorNumber == 0).ToArray();

            Map = new MicroDriveSectorMap(this.Sectors.Where(s => s.Header.HeaderFlag == 0xFF && s.Header.SectorNumber == 0).First());
            Directory = new MicroDriveDirectory(Sectors, Map);
        }

        public MicroDriveCartridge(MicroDriveDirectory Directory, string MediumName)
        {
            Sectors = new MicroDriveSector[255];

            Random rnd = new Random();
            ushort randNum = (ushort)rnd.Next(0, 65535);

            for (int buc = 254; buc > -1; buc--)
                Sectors[buc] = new MicroDriveSector(MediumName, (byte)buc, randNum);

            Map = new MicroDriveSectorMap();

            int currentSector = 245;
            int currentFile = 0;

            byte[] fileContent = Directory.Serialize();
            StoreFile(Map, ref currentSector, ref currentFile, fileContent);

            foreach (var file in Directory.Files)
            {
                fileContent = file.Serialize();
                StoreFile(Map, ref currentSector, ref currentFile, fileContent);
            }

            Map.LastAllocatedSector = (byte)currentSector;

            Sectors[0] = Map.ToSectorZero(MediumName, randNum);

            this.Directory = new MicroDriveDirectory(Sectors, Map);

        }

        private void StoreFile(MicroDriveSectorMap map, ref int currentSector, ref int fileNumber, byte[] fileContent)
        {
            int sectorCount = (int)Math.Ceiling((double)fileContent.Length / 512.0);

            for (int buc = 0; buc < sectorCount; buc++)
            {
                byte[] sectorContent = new byte[512];
                int sectorStart = buc * 512;
                Buffer.BlockCopy(fileContent, sectorStart, sectorContent, 0, Math.Min(512, fileContent.Length - sectorStart));
                var sector = Sectors[currentSector];
                sector.Record.ManagedData = sectorContent;
                sector.Record.FileBlock = (byte)buc;
                sector.Record.FileNumber = (byte)fileNumber;
                sector.Record.ComputeChecksums();
                Sectors[currentSector] = sector;
                map.AssignSector((byte)currentSector, (byte)fileNumber, (byte)buc);

                currentSector = GetFreeBlock(map, (byte)currentSector);
            }

            fileNumber++;
        }

        private byte GetFreeBlock(MicroDriveSectorMap map, byte lastSector)
        {
            int sector = lastSector + 13;

            for (int i = 0; i < MAX_SECTORS; i++)
            {
                if (sector >= MAX_SECTORS)
                    sector -= MAX_SECTORS;

                Debug.WriteLine($"Checking sector {sector}");

                if (map[sector].FileNumber == MicroDriveSectorMap.FREE_SECTOR)
                    return (byte)sector;

                sector++;
            }

            throw new OutOfMemoryException("Not enough space");
        }

        private unsafe void FixSectorChecksums(ref MicroDriveSector Sector)
        {
            Sector.Header.ComputeChecksum();
            Sector.Record.ComputeChecksums();
        }

        public unsafe bool SaveMDV(string OutputFile)
        {
            try
            {
                List<byte> mdv = new List<byte>();

                byte[] preamble = new byte[MDV_PREAMBLE_SIZE];
                preamble[preamble.Length - 2] = 0xFF;
                preamble[preamble.Length - 1] = 0xFF;

                byte[] pad = new byte[MDV_PAD_SIZE];
                Array.Fill(pad, (byte)'Z');

                var s0 = this.Sectors.Where(s => s.Header.HeaderFlag == 0xFF && s.Header.SectorNumber == 0).First();

                byte[] headerData = s0.Header.Serialize();

                MicroDriveHeader hdr;

                fixed (byte* ptr = headerData)
                    hdr = *(MicroDriveHeader*)ptr;

                if (!hdr.Equals(s0.Header))
                    throw new Exception("Header serialization error");

                byte[] recordData = s0.Record.Serialize();

                MicroDriveRecord rec;

                fixed (byte* ptr = recordData)
                    rec = *(MicroDriveRecord*)ptr;

                if (!rec.Equals(s0.Record))
                    throw new Exception("Record serialization error");

                


                mdv.AddRange(preamble);
                mdv.AddRange(headerData);
                mdv.AddRange(preamble);
                mdv.AddRange(recordData);
                mdv.AddRange(pad);

                foreach (var sector in Sectors.Where(s => s.Header.SectorNumber != 0).OrderByDescending(s => s.Header.SectorNumber))
                {
                    headerData = sector.Header.Serialize();
                    recordData = sector.Record.Serialize();

                    fixed (byte* ptr = headerData)
                        hdr = *(MicroDriveHeader*)ptr;

                    if (!hdr.Equals(sector.Header))
                        throw new Exception("Header serialization error");

                    fixed (byte* ptr = recordData)
                        rec = *(MicroDriveRecord*)ptr;

                    if (!rec.Equals(sector.Record))
                        throw new Exception("Record serialization error");

                    mdv.AddRange(preamble);
                    mdv.AddRange(headerData);
                    mdv.AddRange(preamble);
                    mdv.AddRange(recordData);
                    mdv.AddRange(pad);
                }

                File.WriteAllBytes(OutputFile, mdv.ToArray());

                return true;
            }
            catch { return false; }
        }

        public bool SaveMPD(string OutputFile)
        {
            try
            {
                List<byte> mpd = new List<byte>();

                foreach (var sector in Sectors.OrderByDescending(s => s.Header.SectorNumber))
                {
                    byte[] headerData = sector.Header.Serialize();
                    byte[] recordData = sector.Record.Serialize();

                    mpd.AddRange(headerData);
                    mpd.AddRange(recordData);
                }

                File.WriteAllBytes(OutputFile, mpd.ToArray());

                return true;
            }
            catch { return false; }
        }
    }
}
