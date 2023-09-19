using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MicroDriveTools
{
    public static class ConvertTools
    {
        public static unsafe ushort GetUshort(byte* Input)
        {
            return (ushort)(Input[0] << 8 | Input[1]);
        }
        public static unsafe short GetShort(byte* Input)
        {
            return (short)(Input[0] << 8 | Input[1]);
        }
        public static unsafe uint GetUint(byte* Input)
        {
            return (uint)((Input[0] << 24) | (Input[1] << 16) | (Input[2] << 8) | (Input[3] << 0));
        }
        public static unsafe int GetInt(byte* Input)
        {
            return (int)((Input[0] << 24) | (Input[1] << 16) | (Input[2] << 8) | (Input[3] << 0));
        }

        public static unsafe void SetUshort(ushort Input, byte* Output)
        {
            Output[0] = (byte)((Input >> 8) & 0xFF);
            Output[1] = (byte)((Input >> 0) & 0xFF);
        }
        public static unsafe void SetShort(short Input, byte* Output)
        {
            Output[0] = (byte)((Input >> 8) & 0xFF);
            Output[1] = (byte)((Input >> 0) & 0xFF);
        }
        public static unsafe void SetUint(uint Input, byte* Output)
        {
            Output[0] = (byte)((Input >> 24) & 0xFF);
            Output[1] = (byte)((Input >> 16) & 0xFF);
            Output[2] = (byte)((Input >> 8) & 0xFF);
            Output[3] = (byte)((Input >> 0) & 0xFF);
        }
        public static unsafe void SetInt(int Input, byte* Output)
        {
            Output[0] = (byte)((Input >> 24) & 0xFF);
            Output[1] = (byte)((Input >> 16) & 0xFF);
            Output[2] = (byte)((Input >> 8) & 0xFF);
            Output[3] = (byte)((Input >> 0) & 0xFF);
        }
    }
}
