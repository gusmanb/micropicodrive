using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MDToolsUI
{
    public static class Utils
    {
        public static int ReplaceDriveNames(byte[] data, string[] names)
        {
            int occurrences = 0;

            foreach (var name in names)
            {
                byte[] binName = Encoding.ASCII.GetBytes(name);

                occurrences += ReplaceDriveName(data, binName);
            }

            return occurrences;
        }
        public static int ReplaceDriveName(byte[] data, byte[] binName)
        {
            int occurrences = 0;

            byte[] finalName = new byte[4];
            Array.Copy(binName, finalName, binName.Length);

            finalName[3] = Encoding.ASCII.GetBytes("1")[0];

            byte[] replaceName = Encoding.ASCII.GetBytes("mdv1");

            int[] matches = SearchBytePattern(finalName, data);

            foreach (var match in matches)
                Array.Copy(replaceName, 0, data, match, replaceName.Length);

            occurrences += matches.Length;

            finalName[3] = Encoding.ASCII.GetBytes("2")[0];
            replaceName = Encoding.ASCII.GetBytes("mdv2");

            matches = SearchBytePattern(finalName, data);

            foreach (var match in matches)
                Array.Copy(replaceName, 0, data, match, replaceName.Length);

            occurrences += matches.Length;

            return occurrences;
        }

        private static int[] SearchBytePattern(byte[] pattern, byte[] bytes)
        {
            List<int> positions = new List<int>();
            int patternLength = pattern.Length;
            int totalLength = bytes.Length;
            byte firstMatchByte = pattern[0];
            for (int i = 0; i < totalLength; i++)
            {
                if (firstMatchByte == bytes[i] && totalLength - i >= patternLength)
                {
                    byte[] match = new byte[patternLength];
                    Array.Copy(bytes, i, match, 0, patternLength);
                    if (match.SequenceEqual<byte>(pattern))
                    {
                        positions.Add(i);
                        i += patternLength - 1;
                    }
                }
            }
            return positions.ToArray();
        }
    }
}
