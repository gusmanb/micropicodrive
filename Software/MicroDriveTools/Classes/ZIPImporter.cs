using MicroDriveTools.Structs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Tools.Zip;

namespace MicroDriveTools.Classes
{
    public static class ZIPImporter
    {
        public unsafe static MicroDriveDirectory ImportZIPFile(string FileName)
        {
            var storer = ZipArchive.Open(FileName, FileMode.Open);

            MicroDriveDirectory dir = new MicroDriveDirectory();

            foreach (var entry in storer)
            {
                MemoryStream ms = new MemoryStream();
                entry.Extract(ms);
                byte[] fileData = ms.ToArray();

                if (entry.ExtraFieldPresent(0xfb4a))
                {
                    var data = entry.GetExtraField(0xfb4a);

                    if (data != null && data.Count == 1 && data[0].RawData.Length == 72)
                    {
                        MicroDriveZipFileHeader fh = new MicroDriveZipFileHeader();

                        fixed (byte* ptr = data[0].RawData)
                            fh = *((MicroDriveZipFileHeader*)ptr);

                        MicroDriveFile nFile = new MicroDriveFile(fh.FileHeader.FileName, fileData, fh.FileHeader.FileType != 0, fh.FileHeader.DataSpace);
                        dir.AddFile(nFile);
                        continue;
                    }

                }

                MicroDriveFile file = new MicroDriveFile(entry.FullName, fileData);
                dir.AddFile(file);

            }

            return dir;
        }
    }
}
