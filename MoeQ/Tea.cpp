/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include "pch.h"
#include "Tea.h"

int oi_symmetry_encrypt2_len(int nInBufLen);
void oi_symmetry_encrypt2(const byte* pInBuf, int nInBufLen, const byte* pKey, byte* pOutBuf, size_t* pOutBufLen);
bool oi_symmetry_decrypt2(const byte* pInBuf, int nInBufLen, const byte* pKey, byte* pOutBuf, size_t* pOutBufLen);

void Tea::encrypt(const byte* key, const byte* sIn, size_t iLength, ::std::vector<byte>& buffer)
{
	size_t outlen = oi_symmetry_encrypt2_len(iLength);

	if (buffer.capacity() < outlen * 2)
	{
		buffer.resize(outlen * 2);
	}

	oi_symmetry_encrypt2(sIn, iLength, key, buffer.data(), &outlen);

	buffer.resize(outlen);
}

bool Tea::decrypt(const byte* key, const byte* sIn, size_t iLength, ::std::vector<byte>& buffer)
{
	size_t outlen = iLength;

	buffer.resize(outlen * 2);

	if (!oi_symmetry_decrypt2(sIn, iLength, key, buffer.data(), &outlen))
	{
		return false;
	}

	buffer.resize(outlen);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
///


const uint DELTA = 0x9e3779b9;

#define ROUNDS 16
#define LOG_ROUNDS 4
#define SALT_LEN 2
#define ZERO_LEN 7

/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
void TeaEncryptECB(const byte* pInBuf, const byte* pKey, byte* pOutBuf)
{
	uint y, z;
	uint sum;
	uint k[4];
	int i;

	/*plain-text is TCP/IP-endian;*/

	/*GetBlockBigEndian(in, y, z);*/
	y = XBin::Bin2Int(pInBuf);
	z = XBin::Bin2Int(pInBuf + 4);
	/*TCP/IP network byte order (which is big-endian).*/

	k[0] = XBin::Bin2Int(pKey);
	k[1] = XBin::Bin2Int(pKey + 4);
	k[2] = XBin::Bin2Int(pKey + 8);
	k[3] = XBin::Bin2Int(pKey + 12);

	sum = 0;
	for (i = 0; i < ROUNDS; i++)
	{
		sum += DELTA;
		y += ((z << 4) + k[0]) ^ (z + sum) ^ ((z >> 5) + k[1]);
		z += ((y << 4) + k[2]) ^ (y + sum) ^ ((y >> 5) + k[3]);
	}
	
	XBin::Int2Bin(y, pOutBuf);
	XBin::Int2Bin(z, pOutBuf + 4);


	/*now encrypted buf is TCP/IP-endian;*/
}

/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
void TeaDecryptECB(const byte* pInBuf, const byte* pKey, byte* pOutBuf)
{
	uint y, z, sum;
	uint k[4];
	int i;

	/*now encrypted buf is TCP/IP-endian;*/
	/*TCP/IP network byte order (which is big-endian).*/
	y = XBin::Bin2Int(pInBuf);
	z = XBin::Bin2Int(pInBuf + 4);

	k[0] = XBin::Bin2Int(pKey);
	k[1] = XBin::Bin2Int(pKey + 4);
	k[2] = XBin::Bin2Int(pKey + 8);
	k[3] = XBin::Bin2Int(pKey + 12);

	sum = DELTA << LOG_ROUNDS;
	for (i = 0; i < ROUNDS; i++)
	{
		z -= ((y << 4) + k[2]) ^ (y + sum) ^ ((y >> 5) + k[3]);
		y -= ((z << 4) + k[0]) ^ (z + sum) ^ ((z >> 5) + k[1]);
		sum -= DELTA;
	}

	XBin::Int2Bin(y, pOutBuf);
	XBin::Int2Bin(z, pOutBuf + 4);

	/*now plain-text is TCP/IP-endian;*/
}

/*pKeyΪ16byte*/
/*
	����:nInBufLenΪ����ܵ����Ĳ���(Body)����;
	���:����Ϊ���ܺ�ĳ���(��8byte�ı���);
*/
/*TEA�����㷨,CBCģʽ*/
/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
int oi_symmetry_encrypt2_len(int nInBufLen)
{

	int nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero�ĳ���*/;
	int nPadlen;

	/*����Body���ȼ���PadLen,��С���賤�ȱ���Ϊ8byte��������*/
	nPadSaltBodyZeroLen = nInBufLen/*Body����*/ + 1 + SALT_LEN + ZERO_LEN/*PadLen(1byte)+Salt(2byte)+Zero(7byte)*/;
	if ((nPadlen = nPadSaltBodyZeroLen % 8)) /*len=nSaltBodyZeroLen%8*/
	{
		/*ģ8��0�貹0,��1��7,��2��6,...,��7��1*/
		nPadlen = 8 - nPadlen;
	}

	return nPadSaltBodyZeroLen + nPadlen;
}

/*pKeyΪ16byte*/
/*
	����:pInBufΪ����ܵ����Ĳ���(Body),nInBufLenΪpInBuf����;
	���:pOutBufΪ���ĸ�ʽ,pOutBufLenΪpOutBuf�ĳ�����8byte�ı���;
*/
/*TEA�����㷨,CBCģʽ*/
/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
void oi_symmetry_encrypt2(const byte* pInBuf, int nInBufLen, const byte* pKey, byte* pOutBuf, size_t* pOutBufLen)
{

	int nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero�ĳ���*/;
	int nPadlen;
	byte src_buf[8], iv_plain[8] = {0}, * iv_crypt;
	int src_i, i, j;

	/*����Body���ȼ���PadLen,��С���賤�ȱ���Ϊ8byte��������*/
	nPadSaltBodyZeroLen = nInBufLen/*Body����*/ + 1 + SALT_LEN + ZERO_LEN/*PadLen(1byte)+Salt(2byte)+Zero(7byte)*/;
	if ((nPadlen = nPadSaltBodyZeroLen % 8)) /*len=nSaltBodyZeroLen%8*/
	{
		/*ģ8��0�貹0,��1��7,��2��6,...,��7��1*/
		nPadlen = 8 - nPadlen;
	}

	/*srand( (unsigned)time( NULL ) ); ��ʼ�������*/
	/*���ܵ�һ������(8byte),ȡǰ��10byte*/
	src_buf[0] = (((char)rand()) & 0x0f8)/*�����λ��PadLen,����*/ | (char)nPadlen;
	src_i = 1; /*src_iָ��src_buf��һ��λ��*/

	while (nPadlen--) src_buf[src_i++] = (char)rand(); /*Padding*/

	/*come here, src_i must <= 8*/

	iv_crypt = iv_plain; /*make zero iv*/

	*pOutBufLen = 0; /*init OutBufLen*/

	for (i = 1; i <= SALT_LEN;) /*Salt(2byte)*/
	{
		if (src_i < 8)
		{
			src_buf[src_i++] = (char)rand();
			i++; /*i inc in here*/
		}

		if (src_i == 8)
		{
			/*src_i==8*/

			for (j = 0; j < 8; j++) /*����ǰ���ǰ8��byte������(iv_cryptָ���)*/
				src_buf[j] ^= iv_crypt[j];

			/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
			/*����*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);

			for (j = 0; j < 8; j++) /*���ܺ����ǰ8��byte������(iv_plainָ���)*/
				pOutBuf[j] ^= iv_plain[j];

			/*���浱ǰ��iv_plain*/
			for (j = 0; j < 8; j++) iv_plain[j] = src_buf[j];

			/*����iv_crypt*/
			src_i = 0;
			iv_crypt = pOutBuf;
			*pOutBufLen += 8;
			pOutBuf += 8;
		}
	}

	/*src_iָ��src_buf��һ��λ��*/

	while (nInBufLen)
	{
		if (src_i < 8)
		{
			src_buf[src_i++] = *(pInBuf++);
			nInBufLen--;
		}

		if (src_i == 8)
		{
			/*src_i==8*/

			for (j = 0; j < 8; j++) /*����ǰ���ǰ8��byte������(iv_cryptָ���)*/
				src_buf[j] ^= iv_crypt[j];
			/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);

			for (j = 0; j < 8; j++) /*���ܺ����ǰ8��byte������(iv_plainָ���)*/
				pOutBuf[j] ^= iv_plain[j];

			/*���浱ǰ��iv_plain*/
			for (j = 0; j < 8; j++) iv_plain[j] = src_buf[j];

			src_i = 0;
			iv_crypt = pOutBuf;
			*pOutBufLen += 8;
			pOutBuf += 8;
		}
	}

	/*src_iָ��src_buf��һ��λ��*/

	for (i = 1; i <= ZERO_LEN;)
	{
		if (src_i < 8)
		{
			src_buf[src_i++] = 0;
			i++; /*i inc in here*/
		}

		if (src_i == 8)
		{
			/*src_i==8*/

			for (j = 0; j < 8; j++) /*����ǰ���ǰ8��byte������(iv_cryptָ���)*/
				src_buf[j] ^= iv_crypt[j];
			/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);

			for (j = 0; j < 8; j++) /*���ܺ����ǰ8��byte������(iv_plainָ���)*/
				pOutBuf[j] ^= iv_plain[j];

			/*���浱ǰ��iv_plain*/
			for (j = 0; j < 8; j++) iv_plain[j] = src_buf[j];

			src_i = 0;
			iv_crypt = pOutBuf;
			*pOutBufLen += 8;
			pOutBuf += 8;
		}
	}

}


/*pKeyΪ16byte*/
/*
	����:pInBufΪ���ĸ�ʽ,nInBufLenΪpInBuf�ĳ�����8byte�ı���; *pOutBufLenΪ���ջ������ĳ���
		�ر�ע��*pOutBufLenӦԤ�ý��ջ������ĳ���!
	���:pOutBufΪ����(Body),pOutBufLenΪpOutBuf�ĳ���,����ӦԤ��nInBufLen-10;
	����ֵ:�����ʽ��ȷ����true;
*/
/*TEA�����㷨,CBCģʽ*/
/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
bool oi_symmetry_decrypt2(const byte* pInBuf, int nInBufLen, const byte* pKey, byte* pOutBuf, size_t* pOutBufLen)
{

	int nPadLen, nPlainLen;
	byte dest_buf[8], zero_buf[8] = {0};
	const byte* iv_pre_crypt, * iv_cur_crypt;
	int dest_i, i, j;
	//    const char *pInBufBoundary;
	int nBufPos;
	nBufPos = 0;

	if ((nInBufLen % 8) || (nInBufLen < 16)) return false;


	TeaDecryptECB(pInBuf, pKey, dest_buf);

	nPadLen = dest_buf[0] & 0x7/*ֻҪ�����λ*/;

	/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
	i = nInBufLen - 1/*PadLen(1byte)*/ - nPadLen - SALT_LEN - ZERO_LEN; /*���ĳ���*/
	if ((*pOutBufLen < (size_t)i) || (i < 0)) return false;
	*pOutBufLen = i;

	//    pInBufBoundary = pInBuf + nInBufLen; /*���뻺�����ı߽磬���治��pInBuf>=pInBufBoundary*/


	iv_pre_crypt = zero_buf;
	iv_cur_crypt = pInBuf; /*init iv*/

	pInBuf += 8;
	nBufPos += 8;

	dest_i = 1; /*dest_iָ��dest_buf��һ��λ��*/


	/*��Padding�˵�*/
	dest_i += nPadLen;

	/*dest_i must <=8*/

	/*��Salt�˵�*/
	for (i = 1; i <= SALT_LEN;)
	{
		if (dest_i < 8)
		{
			dest_i++;
			i++;
		}
		else if (dest_i == 8)
		{
			/*�⿪һ���µļ��ܿ�*/

			/*�ı�ǰһ�����ܿ��ָ��*/
			iv_pre_crypt = iv_cur_crypt;
			iv_cur_crypt = pInBuf;

			/*���ǰһ������(��dest_buf[]��)*/
			for (j = 0; j < 8; j++)
			{
				if ((nBufPos + j) >= nInBufLen) return false;
				dest_buf[j] ^= pInBuf[j];
			}

			/*dest_i==8*/
			TeaDecryptECB(dest_buf, pKey, dest_buf);

			/*��ȡ����ʱ������ǰһ������(iv_pre_crypt)*/


			pInBuf += 8;
			nBufPos += 8;

			dest_i = 0; /*dest_iָ��dest_buf��һ��λ��*/
		}
	}

	/*��ԭ����*/

	nPlainLen = *pOutBufLen;
	while (nPlainLen)
	{
		if (dest_i < 8)
		{
			*(pOutBuf++) = dest_buf[dest_i] ^ iv_pre_crypt[dest_i];
			dest_i++;
			nPlainLen--;
		}
		else if (dest_i == 8)
		{
			/*dest_i==8*/

			/*�ı�ǰһ�����ܿ��ָ��*/
			iv_pre_crypt = iv_cur_crypt;
			iv_cur_crypt = pInBuf;

			/*�⿪һ���µļ��ܿ�*/

			/*���ǰһ������(��dest_buf[]��)*/
			for (j = 0; j < 8; j++)
			{
				if ((nBufPos + j) >= nInBufLen) return false;
				dest_buf[j] ^= pInBuf[j];
			}

			TeaDecryptECB(dest_buf, pKey, dest_buf);

			/*��ȡ����ʱ������ǰһ������(iv_pre_crypt)*/


			pInBuf += 8;
			nBufPos += 8;

			dest_i = 0; /*dest_iָ��dest_buf��һ��λ��*/
		}
	}

	/*У��Zero*/
	for (i = 1; i <= ZERO_LEN;)
	{
		if (dest_i < 8)
		{
			if (dest_buf[dest_i] ^ iv_pre_crypt[dest_i]) return false;
			dest_i++;
			i++;
		}
		else if (dest_i == 8)
		{
			/*�ı�ǰһ�����ܿ��ָ��*/
			iv_pre_crypt = iv_cur_crypt;
			iv_cur_crypt = pInBuf;

			/*�⿪һ���µļ��ܿ�*/

			/*���ǰһ������(��dest_buf[]��)*/
			for (j = 0; j < 8; j++)
			{
				if ((nBufPos + j) >= nInBufLen) return false;
				dest_buf[j] ^= pInBuf[j];
			}

			TeaDecryptECB(dest_buf, pKey, dest_buf);

			/*��ȡ����ʱ������ǰһ������(iv_pre_crypt)*/


			pInBuf += 8;
			nBufPos += 8;
			dest_i = 0; /*dest_iָ��dest_buf��һ��λ��*/
		}

	}

	return true;
}