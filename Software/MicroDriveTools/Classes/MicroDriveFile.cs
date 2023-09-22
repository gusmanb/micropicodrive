using MicroDriveTools.Structs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MicroDriveTools.Classes
{
    public class MicroDriveFile
    {
        public MicroDriveFileHeader Header { get; private set; }
        public byte[] Data { get; private set; }
        public byte FileNumber { get; private set; }
        internal unsafe MicroDriveFile(MicroDriveSectorMapEntry[] FileMap, MicroDriveSector[] Sectors)
        {
            FileNumber = FileMap[0].FileNumber;

            var currentBlock = Sectors.Where(s => s.Header.HeaderFlag == 0xFF && s.Header.SectorNumber == FileMap[0].SectorNumber).FirstOrDefault();

            if (currentBlock.Header.SectorNumber != FileMap[0].SectorNumber)
                throw new FileNotFoundException($"Cannot find file sector {FileMap[0].SectorNumber}");

            if (currentBlock.Record.FileBlock != FileMap[0].FileBlock || currentBlock.Record.FileNumber != FileMap[0].FileNumber)
                throw new FileNotFoundException($"Cannot find file sector {FileMap[0].SectorNumber}");

            byte* sectorData = currentBlock.Record.Data;

            byte[] headData = new byte[64];
            Marshal.Copy((IntPtr)sectorData, headData, 0, 64);

            MicroDriveFileHeader head = new MicroDriveFileHeader();
            head = *((MicroDriveFileHeader*)sectorData);
            Header = head;

            int len = (int)head.FileLength;

            List<byte> dataBuffer = new List<byte>();

            for (int buc = 0; buc < FileMap.Length; buc++)
            {

                if (len == 0)
                    break;

                currentBlock = Sectors.Where(s => s.Header.HeaderFlag == 0xFF && s.Header.SectorNumber == FileMap[buc].SectorNumber).FirstOrDefault();

                if (currentBlock.Header.SectorNumber != FileMap[buc].SectorNumber)
                    throw new FileNotFoundException($"Cannot find file sector {FileMap[buc].SectorNumber}");

                if (currentBlock.Record.FileBlock != FileMap[buc].FileBlock || currentBlock.Record.FileNumber != FileMap[buc].FileNumber)
                    throw new FileNotFoundException($"Cannot find file sector {FileMap[buc].SectorNumber}");

                int dataLen = 512;
                sectorData = currentBlock.Record.Data;

                if (buc == 0)
                {
                    len -= 64;
                    dataLen -= 64;
                    sectorData += 64;
                }

                byte[] currentData = new byte[Math.Min(dataLen, len)];
                Marshal.Copy((IntPtr)sectorData, currentData, 0, currentData.Length);
                dataBuffer.AddRange(currentData);
                len -= currentData.Length;
            }

            if (len != 0)
                throw new InvalidDataException($"Damaged file {FileMap[0].FileNumber}");

            Data = dataBuffer.ToArray();
        }
        public MicroDriveFile(string FileName, byte[] Data, bool Executable = false, uint DataSpace = 0)
        {
            var header = new MicroDriveFileHeader();

            header.FileName = FileName.Replace(".", "_");
            header.FileLength = (uint)Data.Length + 64;
            header.BackupDate = 3;

            if (Executable)
            {
                header.FileType = 1;
                header.DataSpace = DataSpace;
            }

            Header = header;
            this.Data = Data;
        }

        public void UpdateExecutable(bool IsExecutable, uint DataSpace = 0)
        {
            var hdr = Header;
            hdr.FileType = (byte)(IsExecutable ? 1 : 0);
            hdr.DataSpace = DataSpace;
            Header = hdr;
        }

        public void UpdateFileName(string FileName)
        {
            var hdr = Header;
            hdr.FileName = FileName.Replace(".", "_");
            Header = hdr;
        }

        public void UpdateData(byte[] Data)
        {
            var hdr = Header;
            hdr.FileLength = (uint)Data.Length + 64;
            Header = hdr;
            this.Data = Data;
        }

        public unsafe byte[] Serialize()
        {
            List<byte> buffer = new List<byte>();

            byte[] hData = new byte[64];
            MicroDriveFileHeader hdr = Header;

            fixed(void* ptr = hData)
                Marshal.StructureToPtr(hdr, (IntPtr)ptr, false);
            
            buffer.AddRange(hData);
            buffer.AddRange(Data);
            return buffer.ToArray();
        }
    }
}
