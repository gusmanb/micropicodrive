using MicroDriveTools.Structs;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MicroDriveTools.Classes
{
    public class MicroDriveDirectory
    {
        List<MicroDriveFile> files = new List<MicroDriveFile>();
        List<MicroDriveFileHeader> directoryEntries = new List<MicroDriveFileHeader>();

        public MicroDriveFile[] Files { get { return files.ToArray(); } }
        public MicroDriveFileHeader[] DirectoryEntries { get { return directoryEntries.ToArray(); } }

        internal unsafe MicroDriveDirectory(MicroDriveSector[] Sectors, MicroDriveSectorMap Map)
        {
            List<MicroDriveFile> fileBuffer = new List<MicroDriveFile>();

            var directoryMap = Map.GetFileMap(0);

            if (directoryMap == null || directoryMap.Length == 0)
                throw new FileNotFoundException("Cannot find directory file.");

            var directory = new MicroDriveFile(directoryMap, Sectors);

            if(directory.Data.Length % 64 != 0)
                throw new InvalidDataException($"Damaged directory file.");

            int fileCount = (int)directory.Data.Length / 64;

            var data = directory.Data;

            if (data.Length > 0)
            {
                fixed (byte* ptr = &data[0])
                {
                    MicroDriveFileHeader* headerPtr = (MicroDriveFileHeader*)ptr;

                    for (int buc = 0; buc < fileCount; buc++)
                        directoryEntries.Add(headerPtr[buc]);
                }

                for (int buc = 0; buc < fileCount; buc++)
                {
                    var map = Map.GetFileMap((byte)(buc + 1));

                    if (map == null || map.Length == 0)
                        throw new FileNotFoundException($"Cannot find file {buc} map.");

                    var file = new MicroDriveFile(map, Sectors);
                    fileBuffer.Add(file);
                }
            }

            files.AddRange(fileBuffer);
        }

        public MicroDriveDirectory() { }

        public bool AddFile(MicroDriveFile File) 
        {
            if (files.Any(f => f.Header.FileName == File.Header.FileName))
                return false;

            files.Add(File);
            var hdr = File.Header;
            hdr.BackupDate = 0;
            directoryEntries.Add(hdr);

            return true;
        }

        public bool RemoveFile(string FileName) 
        {
            var idx = files.FindIndex(f => f.Header.FileName == FileName);

            if(idx == -1) 
                return false;

            files.RemoveAt(idx);
            directoryEntries.RemoveAt(idx);

            return true;
        }
        
        public unsafe byte[] Serialize()
        {
            int len = (files.Count + 1) * 64;

            byte[] data = new byte[len];

            MicroDriveFileHeader directoryHeader = new MicroDriveFileHeader();
            directoryHeader.FileLength = (uint)len;

            fixed (byte* ptr = data)
            {
                MicroDriveFileHeader* hdrPtr = (MicroDriveFileHeader*)ptr;
                *hdrPtr = directoryHeader;
                hdrPtr++;

                for (int buc = 0; buc < directoryEntries.Count; buc++)
                {
                    *hdrPtr = directoryEntries[buc];
                    hdrPtr++;
                }
            }

            return data;
        }
    }
}
