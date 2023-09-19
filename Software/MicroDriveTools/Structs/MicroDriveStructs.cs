using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace MicroDriveTools.Structs
{

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct MicroDriveSector
    {
        public MicroDriveHeader Header;
        public MicroDriveRecord Record;

        public MicroDriveSector() { }
        public unsafe MicroDriveSector(string MediumName, byte SectorNumber, ushort MediumId)
        {
            Header.HeaderFlag = 0xFF;
            Header.SectorNumber = SectorNumber;
            Header.MediumName = MediumName;
            Header.MediumId = MediumId;
            Header.ComputeChecksum();
            Record.MakeEmpty();
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public unsafe struct MicroDriveHeader
    {
        public byte HeaderFlag { get { return HeaderData[0]; } internal set { HeaderData[0] = value; } }
        public byte SectorNumber { get { return HeaderData[1]; } internal set { HeaderData[1] = value; } }
        public string MediumName 
        { 
            get 
            {
                fixed (byte* ptr = &HeaderData[2])
                {
                    byte[] data = new byte[10];
                    Marshal.Copy((IntPtr)ptr, data, 0, 10);
                    return Encoding.ASCII.GetString(data);
                }
            }

            internal set
            {
                string finalName = value.PadRight(10, ' ');
                finalName = finalName.Substring(0, 10);
                byte[] nameData = Encoding.ASCII.GetBytes(finalName);

                fixed (byte* ptr = &HeaderData[2])
                    Marshal.Copy(nameData, 0, (IntPtr)ptr, 10);
            }
        }
        public ushort MediumId 
        { 
            get { return (ushort)(HeaderData[12] << 8 | HeaderData[13]); } 
            set 
            {
                HeaderData[12] = (byte)(value >> 8);
                HeaderData[13] = (byte)(value & 0xFF);
            } 
        }
        public fixed byte HeaderData[14];
        public ushort Checksum;

        public void ComputeChecksum()
        {
            ushort computedChecksum = 0;

            for (int hpos = 0; hpos < 14; hpos++)
                computedChecksum += HeaderData[hpos];

            computedChecksum += 0x0f0f;
            Checksum = computedChecksum;
        }

        public byte[] Serialize()
        {
            byte[] data = new byte[16];

            fixed(MicroDriveHeader* ptr = &this) 
                Marshal.Copy((IntPtr)ptr, data, 0, 16);
            
            return data;
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public unsafe struct MicroDriveRecord
    {
        public fixed byte HeaderData[2];
        public byte FileNumber { get { return HeaderData[0]; } internal set { HeaderData[0] = value; } }
        public byte FileBlock { get { return HeaderData[1]; } internal set { HeaderData[1] = value; } }
        public ushort HeaderChecksum;
        public fixed byte FilePreamble[8];
        public fixed byte Data[512];
        public byte[] ManagedData 
        { 
            get 
            {
                byte[] data = new byte[512];
                fixed(byte* ptr = Data)
                    Marshal.Copy((IntPtr)ptr, data, 0, 512);

                return data;
            } 
            set 
            {
                byte[] finalData = new byte[512];
                Buffer.BlockCopy(value, 0, finalData, 0, value.Length);

                fixed (byte* ptr = Data)
                    Marshal.Copy(finalData, 0, (IntPtr)ptr, 512);
            } 
        }
        public ushort DataChecksum;
        public fixed byte ExtraBytes[84];
        public ushort ExtraBytesChecksum;

        public void ComputeChecksums() 
        {
            ushort computedChecksum = 0;

            for (int hrBuc = 0; hrBuc < 2; hrBuc++)
                computedChecksum += HeaderData[hrBuc];

            computedChecksum += 0x0f0f;
            HeaderChecksum = computedChecksum;

            computedChecksum = 0;

            for (int hdBuc = 0; hdBuc < 512; hdBuc++)
                computedChecksum += Data[hdBuc];

            computedChecksum += 0x0f0f;
            DataChecksum = computedChecksum;

            for (int bExtra = 0; bExtra < 84; bExtra++)
                ExtraBytes[bExtra] = bExtra % 2 == 0 ? (byte)0xAA : (byte)0x55;

            if (ExtraBytesChecksum != 0x3b19)
                ExtraBytesChecksum = 0x3b19;
        }
        public void MakeEmpty()
        {
            FileNumber = 0xFD;
            FileBlock = 0;

            ushort computedChecksum = 0;

            for (int hrBuc = 0; hrBuc < 2; hrBuc++)
                computedChecksum += HeaderData[hrBuc];

            FilePreamble[6] = 0xFF;
            FilePreamble[7] = 0xFF;

            for (int buc = 0; buc < 512; buc++)
                Data[buc] = (byte)(buc % 2 == 0 ? 0xAA : 0x55);

            ComputeChecksums();
        }
        public byte[] Serialize()
        {
            byte[] data = new byte[612];

            fixed (MicroDriveRecord* ptr = &this)
                Marshal.Copy((IntPtr)ptr, data, 0, 612);

            return data;
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct MicroDriveSectorMapEntry
    {
        public byte SectorNumber;
        public byte FileNumber;
        public byte FileBlock;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public unsafe struct MicroDriveFileHeader
    {

        public const byte DATA_FILE = 0x00;
        public const byte EXE_FILE = 0x01;

        public fixed byte FileLengthBytes[4];
        public byte FileAccess;
        public byte FileType;
        public fixed byte FileInfo[8];
        public fixed byte FileNameData[38];
        public fixed byte FileUpdateDate[4];
        public fixed byte FileReferenceDate[4];
        public fixed byte FileBackupDate[4];

        public uint FileLength 
        {
            get 
            {
                fixed(byte* ptr = FileLengthBytes)
                    return ConvertTools.GetUint(ptr); 
            }

            set
            {
                fixed (byte* ptr = FileLengthBytes)
                    ConvertTools.SetUint(value, ptr);
            }
        }

        public uint FileDataLength { get { return FileLength - 64; } }

        public uint DataSpace
        {
            get
            {
                fixed (byte* ptr = FileInfo)
                    return ConvertTools.GetUint(ptr);
            }

            set
            {
                fixed (byte* ptr = FileInfo)
                    ConvertTools.SetUint(value, ptr);
            }
        }

        public uint ExtraInfo
        {
            get
            {
                fixed (byte* ptr = FileInfo)
                    return ConvertTools.GetUint(ptr + 4);
            }

            set
            {
                fixed (byte* ptr = FileInfo)
                    ConvertTools.SetUint(value, ptr + 4);
            }
        }

        public string FileName
        {
            get 
            {
                fixed (byte* ptr = FileNameData)
                {
                    var len = ConvertTools.GetUshort(ptr);
                    return new string((sbyte*)(ptr + 2), 0, len);
                }
            }

            set 
            {

                string val = value ?? "";

                if(val.Length > 36)
                    throw new ArgumentException("Name too long");

                byte[] nameBytes = new byte[36];
                byte[] strBytes = Encoding.ASCII.GetBytes(val);
                Array.Copy(strBytes, nameBytes, strBytes.Length);

                fixed (byte* ptr = FileNameData)
                {
                    ConvertTools.SetUshort((ushort)strBytes.Length, ptr);
                    
                    for(int buc = 2; buc < 38; buc++)
                        ptr[buc] = nameBytes[buc - 2];
                }
            }
        }

        public uint UpdateDate
        {
            get
            {
                fixed (byte* ptr = FileUpdateDate)
                    return ConvertTools.GetUint(ptr);
            }

            set
            {
                fixed (byte* ptr = FileUpdateDate)
                    ConvertTools.SetUint(value, ptr);
            }
        }

        public uint ReferenceDate
        {
            get
            {
                fixed (byte* ptr = FileReferenceDate)
                    return ConvertTools.GetUint(ptr);
            }

            set
            {
                fixed (byte* ptr = FileReferenceDate)
                    ConvertTools.SetUint(value, ptr);
            }
        }

        public uint BackupDate
        {
            get
            {
                fixed (byte* ptr = FileBackupDate)
                    return ConvertTools.GetUint(ptr);
            }

            set
            {
                fixed (byte* ptr = FileBackupDate)
                    ConvertTools.SetUint(value, ptr);
            }
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public unsafe struct MicroDriveZipFileHeader
    {
        public uint longId;
        public uint extraId;
        public MicroDriveFileHeader FileHeader;
    }
}
