using System;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace TischSharp
{
	class SharpBlob : BasicBlob
	{
		public byte[] serialize()
		{
			byte[] posX = Encoding.ASCII.GetBytes(Convert.ToString(pos.x));
			byte[] posY = Encoding.ASCII.GetBytes(" "+Convert.ToString(pos.y));
			byte[] bSize = Encoding.ASCII.GetBytes(" "+Convert.ToString(1));
			byte[] bId = Encoding.ASCII.GetBytes(" "+Convert.ToString(id));
			byte[] bPid = Encoding.ASCII.GetBytes(" "+Convert.ToString(pid));
			byte[] peakX = Encoding.ASCII.GetBytes(" "+Convert.ToString(pos.x));
			byte[] peakY = Encoding.ASCII.GetBytes(" "+Convert.ToString(pos.y));
			byte[] axis1X = Encoding.ASCII.GetBytes(" "+Convert.ToString(2));
			byte[] axis1Y = Encoding.ASCII.GetBytes(" "+Convert.ToString(0));
			byte[] axis2X = Encoding.ASCII.GetBytes(" "+Convert.ToString(0));
			byte[] axis2Y = Encoding.ASCII.GetBytes(" "+Convert.ToString(1));
			StackArray stack = new StackArray(posX.Length+posY.Length+bSize.Length+bId.Length+bPid.Length+peakX.Length+peakY.Length+axis1X.Length+axis1Y.Length+axis2X.Length+axis2Y.Length);
			stack.push(posX);
			stack.push(posY);
			stack.push(bSize);
			stack.push(bId);
			stack.push(bPid);
			stack.push(peakX);
			stack.push(peakY);
			stack.push(axis1X);
			stack.push(axis1Y);
			stack.push(axis2X);
			stack.push(axis2Y);

			return stack.get();
		}
	}

	class StackArray
	{
		private byte[] array;
		private int sp = 0;
		
		public StackArray(int size)
		{
			array = new byte[size];
		}

		public int push(byte[] cpArr)
		{
			cpArr.CopyTo(array, sp);
			sp += cpArr.Length;
			return sp;
		}

		public byte[] get()
		{
			return array;
		}

	}

	class GestureDaemon
	{
		static IPEndPoint gestureIP = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 0x7AB1);
		static Socket gestureSock = new Socket(AddressFamily.InterNetwork,SocketType.Dgram, ProtocolType.Udp);
		static byte[] bframe = Encoding.ASCII.GetBytes("frame ");
		static byte[] bfinger = Encoding.ASCII.GetBytes("finger ");
		static byte[] endl = Encoding.ASCII.GetBytes("\n");
		
		public static void SendFrame(int num)
		{
			byte[] bnum = Encoding.ASCII.GetBytes(Convert.ToString(num));
			byte[] packet = new byte[bframe.Length+bnum.Length+endl.Length];
			bframe.CopyTo(packet, 0);
			bnum.CopyTo(packet, bframe.Length);
			endl.CopyTo(packet, bframe.Length+bnum.Length);
			gestureSock.SendTo(packet, gestureIP);
		}

		public static void SendFingerBlob(SharpBlob blob)
		{
			byte[] payload = blob.serialize();
			byte[] packet = new byte[bfinger.Length+payload.Length+endl.Length];
			bfinger.CopyTo(packet, 0);
			payload.CopyTo(packet, bfinger.Length);
			endl.CopyTo(packet, bfinger.Length+payload.Length);
			gestureSock.SendTo(packet, gestureIP);
		}

	}
}
