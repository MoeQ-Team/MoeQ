#include "Tlv.h"

#define FUNC(TlvName, ...) uint16_t Tlv::Tlv##TlvName(byte *bin, const uint len, ##__VA_ARGS__)

FUNC(001, uint QQ, uint Time)
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(1); // _ip_ver
    Pack.SetBin_(Utils::GetRandomBin(4), 4); // random
    Pack.SetInt(QQ);
    Pack.SetInt(Time);
    Pack.SetInt(0); // unknown
    Pack.SetShort(0); // const
    return Pack.Pack(0x0001);
}

FUNC(002, const char *code, const char VieryToken1[4])
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(0); // _sigVer
    Pack.SetShort(strlen(code));
    Pack.SetStr(code);
    Pack.SetShort(strlen(VieryToken1)); // len
    Pack.SetStr(VieryToken1);
    return Pack.Pack(0x0002);
}

FUNC(008)
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(0);
    Pack.SetInt(0x0804);
    Pack.SetShort(0);
    return Pack.Pack(0x0008);
}

FUNC(018, const uint QQ)
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(1); // _ping_version
    Pack.SetInt(1536); // _sso_version
    Pack.SetInt(16);
    Pack.SetInt(0);
    Pack.SetInt(QQ);
    Pack.SetShort(0);
    Pack.SetShort(0); // const
    return Pack.Pack(0x0018);
}

FUNC(100, const uint SUB_APPID, const bool IsFreshSkey)
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(1);  //_db_buf_ver
    Pack.SetInt(0x15); //_sso_ver
    Pack.SetInt(0X10); // appid
    Pack.SetInt(IsFreshSkey ? SUB_APPID : 1); // Fresh D2Key
    Pack.SetInt(0);
    Pack.SetInt(IsFreshSkey ? 0x021410E0 : 0X001000C0);
    return Pack.Pack(0x0100);
}

FUNC(104, const byte VieryToken2[36])
{
    ::TlvPack Pack(bin, len);
    Pack.SetBin(VieryToken2, 36);
    return Pack.Pack(0x0104);
    ;
}

FUNC(106, const uint QQ, const char *QQ_Str, const byte md5[16], const byte md52[16], const byte TGTKey[16], const byte GUID[16], const uint Time, const uint SUB_APPID, bool emp)
{
    ::Pack _Pack;
    _Pack.SetShort(4); //_TGTGTVer
    _Pack.SetInt(Utils::GetRandom(129312, 123128723));
    _Pack.SetInt(0x15); //_SSoVer
    _Pack.SetInt(0X10);
    _Pack.SetInt(0);
    _Pack.SetLong(QQ);
    _Pack.SetInt(Time);
    _Pack.SetInt(0);
    _Pack.SetByte(1);
    _Pack.SetBin(md5, 16);
    _Pack.SetBin(TGTKey, 16);
    _Pack.SetInt(emp ? 0x85 : 0);
    _Pack.SetByte(1);
    _Pack.SetBin(GUID, 16);
    _Pack.SetInt(SUB_APPID);
    _Pack.SetInt(1);
    _Pack.SetShort(strlen(QQ_Str));
    _Pack.SetStr(QQ_Str);
    _Pack.SetShort(0);

    std::vector<byte> data;
    Tea::encrypt(md52, _Pack.GetAll(), _Pack.Length(), data);
    delete[] _Pack.GetAll();

    ::TlvPack Pack(bin, len);
    Pack.SetBin(&data);
    return Pack.Pack(0x0106);
    ;
}

FUNC(107)
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(0);
    Pack.SetByte(0);
    Pack.SetShort(0);
    Pack.SetByte(1);
    return Pack.Pack(0x0107);
    ;
}

FUNC(108, const byte ksid[16])
{
    ::TlvPack Pack(bin, len);
    Pack.SetBin(ksid, 16);
    return Pack.Pack(0x0108);
}

FUNC(109, const char *IMEI)
{
    ::TlvPack Pack(bin, len);
    Pack.SetMD5((byte *)IMEI, strlen(IMEI));
    return Pack.Pack(0x0109);
}

FUNC(116)
{
    ::TlvPack Pack(bin, len);
    Pack.SetByte(0);         // _ver
    Pack.SetInt(0x0AF7FF7C); // mMiscBitmap
    Pack.SetInt(0x00010400); // mSubSigMap
    Pack.SetByte(1);         // arr length
    Pack.SetInt(0x5F5E10E2);
    return Pack.Pack(0x0116);
}

FUNC(124, const char *os_type, const char *os_version, const unsigned short _network_type, const char *_apn, const char *NetworkName)
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(strlen(os_type));
    Pack.SetStr(os_type);
    Pack.SetShort(strlen(os_version));
    Pack.SetStr(os_version);
    Pack.SetShort(_network_type);
    Pack.SetShort(strlen(NetworkName));
    Pack.SetStr(NetworkName);
    Pack.SetShort(0);
    Pack.SetShort(strlen(_apn));
    Pack.SetStr(_apn);
    return Pack.Pack(0x0124);
}

FUNC(128, const char *_device, const char *Brand, const byte GUID[16])
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(0);
    Pack.SetByte(0);
    Pack.SetByte(1);
    Pack.SetByte(0);
    Pack.SetInt(0x10000000);
    Pack.SetShort(strlen(_device));
    Pack.SetStr(_device);
    Pack.SetShort(16);
    Pack.SetBin(GUID, 16);
    Pack.SetShort(strlen(Brand));
    Pack.SetStr(Brand);
    return Pack.Pack(0x0128);
}

FUNC(141, const char *NetworkName, const char *apn)
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(1);
    Pack.SetShort(strlen(NetworkName));
    Pack.SetStr(NetworkName);
    Pack.SetShort(2);
    Pack.SetShort(strlen(apn));
    Pack.SetStr(apn);
    return Pack.Pack(0x0141);
}

FUNC(142, const char *AndroidQQ_APKID)
{
    ::TlvPack Pack(bin, len);
    Pack.SetInt(strlen(AndroidQQ_APKID));
    Pack.SetStr(AndroidQQ_APKID);
    return Pack.Pack(0x0142);
}

FUNC(144, const byte TGTKey[16], const char *IMEI, const char *os_type, const char *os_version, const unsigned short _network_type, const char *_apn, const char *NetworkName, const char *_device, const char *Brand, const byte GUID[16])
{
    ::Pack _Pack(500);
    _Pack.SetShort(5);

#define PackTlv(TlvName, ...) _Pack.Skip(Tlv::Tlv##TlvName(_Pack.GetCurrentPoint(), _Pack.GetLeftSpace(), ##__VA_ARGS__))

    PackTlv(109, IMEI);
    PackTlv(52D);
    PackTlv(124, os_type, os_version, _network_type, _apn, NetworkName);
    PackTlv(128, _device, Brand, GUID);
    PackTlv(16E, _device);

#undef PackTlv

    std::vector<byte> data;
    Tea::encrypt(TGTKey, _Pack.GetAll(), _Pack.Length(), data);
    delete[] _Pack.GetAll();

    ::TlvPack Pack(bin, len);
    Pack.SetBin(&data);
    return Pack.Pack(0x0144);
}

FUNC(145, const byte GUID[16])
{
    ::TlvPack Pack(bin, len);
    Pack.SetBin(GUID, 16);
    return Pack.Pack(0X0145);
}

FUNC(147, const char *QQ_VERSION, const byte AndroidQQ_ASIG[16])
{
    ::TlvPack Pack(bin, len);
    Pack.SetInt(0x10);
    Pack.SetShort(strlen(QQ_VERSION));
    Pack.SetStr(QQ_VERSION);
    Pack.SetShort(16);
    Pack.SetBin(AndroidQQ_ASIG, 16);
    return Pack.Pack(0X0147);
}

FUNC(154, const uint SsoSeq)
{
    ::TlvPack Pack(bin, len);
    Pack.SetInt(SsoSeq);
    return Pack.Pack(0X0154);
}

FUNC(16A, const byte *token_16A)
{
    ::TlvPack Pack(bin, len);
    Pack.SetBin(token_16A, 56);
    return Pack.Pack(0X016A);
}

FUNC(16E, const char *_device)
{
    ::TlvPack Pack(bin, len);
    Pack.SetStr(_device);
    return Pack.Pack(0X016E);
    ;
}

FUNC(174, const char *SmsToken)
{
    ::TlvPack Pack(bin, len);
    Pack.SetStr(SmsToken);
    return Pack.Pack(0X0174);
}

FUNC(177, const uint build_time, const char *SDK_VERSION)
{
    ::TlvPack Pack(bin, len);
    Pack.SetByte(1);
    Pack.SetInt(build_time);
    Pack.SetShort(strlen(SDK_VERSION));
    Pack.SetStr(SDK_VERSION);
    return Pack.Pack(0x0177);
}

FUNC(17A)
{
    ::TlvPack Pack(bin, len);
    Pack.SetInt(9);
    return Pack.Pack(0X017A);
}

FUNC(17C, const char *code)
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(strlen(code));
    Pack.SetStr(code);
    return Pack.Pack(0X017C);
}

FUNC(187)
{
    ::TlvPack Pack(bin, len);
    Pack.SetBin_(Utils::GetRandomBin(16), 16);
    return Pack.Pack(0X0187);
}

FUNC(188, const char *IMEI)
{
    ::TlvPack Pack(bin, len);
    Pack.SetMD5((byte *)IMEI, strlen(IMEI));
    return Pack.Pack(0X0188);
}

// CodeType 验证码类型 0x01:字母 0x82:滑块
FUNC(191, const byte CodeType)
{
    ::TlvPack Pack(bin, len);
    Pack.SetByte(CodeType);
    return Pack.Pack(0X0191);
}

FUNC(193, const char *Ticket)
{
    ::TlvPack Pack(bin, len);
    Pack.SetStr(Ticket);
    return Pack.Pack(0X0193);
}

FUNC(194)
{
    ::TlvPack Pack(bin, len);
    Pack.SetBin_(Utils::GetRandomBin(16), 16);
    return Pack.Pack(0X0194);
}

FUNC(197)
{
    ::TlvPack Pack(bin, len);
    Pack.SetByte(0);
    return Pack.Pack(0X0197);
}

FUNC(198)
{
    ::TlvPack Pack(bin, len);
    Pack.SetByte(0);
    return Pack.Pack(0X0198);
}

FUNC(202, const char *BSSID, const char *WiFi_name)
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(16);
    Pack.SetMD5((byte *)BSSID, strlen(BSSID));
    Pack.SetShort(strlen(WiFi_name));
    Pack.SetStr(WiFi_name);
    return Pack.Pack(0X0202);
}

FUNC(400, const long long QQ, const byte GUID[16], const uint Time, const byte *token_403)
{
    ::Pack _Pack;
    _Pack.SetShort(1);
    _Pack.SetLong(QQ);
    _Pack.SetBin(GUID, 16);
    _Pack.SetStr_(Utils::GetRandomLetter(16));
    _Pack.SetInt(16);
    _Pack.SetInt(1);
    _Pack.SetInt(Time);
    _Pack.SetBin(token_403, 8);

    std::vector<byte> data;
    Tea::encrypt(GUID, _Pack.GetAll(), _Pack.Length(), data); //key is unknown
    delete[] _Pack.GetAll();

    ::TlvPack Pack(bin, len);
    Pack.SetBin(&data);
    return Pack.Pack(0X0400);
}

FUNC(401, const byte GUID[16], const byte *token_402)
{
    ::TlvPack Pack(bin, len);
    byte b[32];
    memcpy(b, GUID, 16);
    memcpy(b + 15, Utils::GetRandomBin(16), 16);
    memcpy(b + 31, token_402, 1);
    Pack.SetMD5(b, 24);
    return Pack.Pack(0X0401);
}

FUNC(511, const char **domainList, const byte ListSize)
{
    ::TlvPack Pack(bin, len);
    Pack.SetShort(ListSize);
    for (size_t i = 0; i < ListSize; i++)
    {
        Pack.SetByte(1);
        Pack.SetShort(strlen(domainList[i]));
        Pack.SetStr(domainList[i]);
    }
    return Pack.Pack(0X0511);
}

FUNC(516)
{
    ::TlvPack Pack(bin, len);
    Pack.SetInt(0);
    return Pack.Pack(0X0516);
}

FUNC(521)
{
    //未知组包
    ::TlvPack Pack(bin, len);
    Pack.SetInt(0);
    Pack.SetShort(0);
    return Pack.Pack(0X0521);
}

FUNC(525, const uint QQ, const char *IP, const uint Time, const uint SUB_APPID, const bool IsEmp)
{
    ::TlvPack Pack(bin, len);
    Pack.SetInt(0x00010536);
    Pack.SetByte(0);
    Pack.SetByte(2);
    Pack.SetByte(1);
    Pack.SetByte(0); // ArrLegth

    /*  
    Pack.SetInt(0);
    Pack.SetInt(QQ);
    Pack.SetByte(4);
    Pack.SetBin_(XBin::IP2Bin(IP), 4);
    Pack.SetInt(Time);
    Pack.SetInt(SUB_APPID);

    Pack.SetInt(0);
    Pack.SetInt(QQ);
    Pack.SetByte(4);
    Pack.SetBin_(XBin::IP2Bin(IP), 4);
    Pack.SetInt(Time);
    Pack.SetInt(SUB_APPID);

    Pack.SetInt(0);
    Pack.SetInt(QQ);
    Pack.SetByte(4);
    Pack.SetBin_(XBin::IP2Bin(IP), 4);
    Pack.SetInt(Time);
    Pack.SetInt(SUB_APPID);
    */

    return Pack.Pack(0X0525);
}

FUNC(52D)
{
    ::TlvPack Pack(bin, len);
    Protobuf PB;
    PB.WriteStr(1,u8"unknown");
    PB.WriteStr(2,u8"");
    PB.WriteStr(3,u8"REL");
    PB.WriteStr(4,u8"V816.0.24.2.20.DEV");
    PB.WriteStr(5,u8"Xiaomi/fuxi/fuxi:13/TKQ1.221114.001/V816.0.24.2.20.DEV:user/release-keys");
    PB.WriteStr(6,u8"2eeb42bb-2cae-484b-9005-0da4bac22386");
    PB.WriteStr(7,u8"4f86468ca2f7ecd7");
    PB.WriteStr(8,u8"");
    PB.WriteStr(9,u8"V816.0.24.2.20.DEV");
    Pack.SetBinEx_(PB.Pack());
    return Pack.Pack(0x052D);
}

FUNC(542)
{
    ::TlvPack Pack(bin, len);
    Pack.SetByte(0x4A);
    Pack.SetByte(7);
    Pack.SetBin((const byte*)"\x60\x01\x78\x01\x80\x01\x01", 7);
    return Pack.Pack(0X0542);
}

FUNC(544)
{
    //Unknown
    ::TlvPack Pack(bin, len);
    Pack.SetBin((const byte *)"\x0C\x17\x04\x04\x81\xA2\x37\x3A\x9D\xA5\xB3\xC8\x24\x37\x15\x91\x8F\xFC\xE3\xF2\x21\xF0\x1D\xB1\x1F\x16\x43\x5C\x66\x71\xED\xCA\xC5\x8A\x63\x3C\x42\xF6\x0B\xBA\xE0\x2D\x5B\xEC\x9A\x2B\x0D\x6E\x58\x34\x6D\x9C\x3E\x41\x3B\x7D\x9F\x91\xBC\xCF\x00\x87\x8B\x83\xD8\x0F\x5F\x82", 69);
    return Pack.Pack(0x0544);
}

FUNC(545)
{
    //算法未知 在so层中
    ::TlvPack Pack(bin, len);
    Pack.SetBin_((byte *)XBin::Bin2Hex(Utils::GetRandomBin(18), 18), 36); // IMEI
    return Pack.Pack(0x0545);
}

// For 547&548
struct PowValue
{
    int version;
    int checkType;     // 1 用原文干密文 2 用密文gan次数
    int algorithmType; // 1 sha256 2 sm3
    int hasHashResult;

    int baseCount;
    int filling[2];

    int originSize;
    byte *origin;

    int cpSize;
    byte *cp;

    int sm3Size;
    byte *sm;

    int hashResultSize;
    byte *hashResult;

    int cost;
    int cnt;
};

void PackPowValue(TlvPack *Pack, const PowValue *PowValue)
{
    Pack->SetByte(PowValue->version);
    Pack->SetByte(PowValue->checkType);
    Pack->SetByte(PowValue->algorithmType);
    Pack->SetByte(PowValue->hasHashResult);
    Pack->SetShort(PowValue->baseCount);
    for (int fill : PowValue->filling)
        Pack->SetByte(fill);
    Pack->SetShort(PowValue->originSize);
    Pack->SetBin_(PowValue->origin, PowValue->originSize);
    Pack->SetShort(PowValue->cpSize);
    Pack->SetBin_(PowValue->cp, PowValue->cpSize);
    Pack->SetShort(PowValue->sm3Size);
    Pack->SetBin_(PowValue->sm, PowValue->sm3Size);
    if (PowValue->hasHashResult == 1)
    {
        Pack->SetShort(PowValue->hashResultSize);
        Pack->SetBin_(PowValue->hashResult, PowValue->hashResultSize);
        Pack->SetInt(PowValue->cost);
        Pack->SetInt(PowValue->cnt);
    }
}

FUNC(547, const byte *ClientPow)
{
    ::TlvPack Pack(bin, len);
    ::ByteInputStream ByteInputStream(ClientPow);
    PowValue PowValue;
    PowValue.version = ByteInputStream.GetByte();
    PowValue.checkType = ByteInputStream.GetByte();
    PowValue.algorithmType = ByteInputStream.GetByte();
    PowValue.hasHashResult = ByteInputStream.GetByte();
    PowValue.baseCount = ByteInputStream.GetShort();
    for (size_t i = 0; i < 2; i++)
    {
        PowValue.filling[i] = ByteInputStream.GetByte();
    }
    PowValue.originSize = ByteInputStream.GetShort();
    PowValue.origin = new byte[PowValue.originSize];
    memcpy(PowValue.origin, ByteInputStream.GetBin(PowValue.originSize), PowValue.originSize);
    PowValue.cpSize = ByteInputStream.GetShort();
    PowValue.cp = new byte[PowValue.cpSize];
    memcpy(PowValue.cp, ByteInputStream.GetBin(PowValue.cpSize), PowValue.cpSize);
    PowValue.sm3Size = ByteInputStream.GetShort();
    PowValue.sm = new byte[PowValue.sm3Size];
    memcpy(PowValue.sm, ByteInputStream.GetBin(PowValue.sm3Size), PowValue.sm3Size);
    /*
        if (PowValue.hasHashResult == 1)
        {
            PowValue.hashResultSize = ByteInputStream.GetShort();
            PowValue.hashResult = new byte[PowValue.hashResultSize];
            memcpy(PowValue.hashResult, ByteInputStream.GetBin(PowValue.hashResultSize), PowValue.hashResultSize);
        }
    */
    PowValue.cost = ByteInputStream.GetInt();
    PowValue.cnt = ByteInputStream.GetInt();

    long currentTimeMillis = Utils::CurrentTimeMillis();

    PowValue.cost = 0;
    PowValue.cnt = 0;

    switch (PowValue.checkType)
    {
    case 1:
    {
        int originSize = PowValue.originSize;
        byte *origin = new byte[originSize];
        memcpy(origin, PowValue.origin, PowValue.originSize);
        switch (PowValue.algorithmType)
        {
        case 1:
            PowValue.hashResultSize = 32;
            PowValue.hashResult = new byte[PowValue.hashResultSize];
            while (true)
            {
                Utils::Sha256(origin, originSize, PowValue.hashResult);

                if ([&]()
                    { 
                    if (PowValue.baseCount > 32)
                        return false;
                    int iv = 255;
                    int i = 0;
                    while (iv >= 0 && i < PowValue.baseCount)
                    {
                        if ((PowValue.hashResult[iv / 8] & (1 << (iv % 8))) != 0)
                            return false;
                        iv--;
                        i++;
                    }
                    return true; }())
                    break;

                PowValue.cnt++;
                BigInteger::AddOne(PowValue.hashResult, PowValue.hashResultSize);
            }
            break;
        case 2:
            throw "hash func not support sm3";
            break;
        default:
            break;
        }
        delete[] origin;
    }
    break;
    case 2:
        PowValue.hashResultSize = PowValue.originSize;
        PowValue.hashResult = new byte[PowValue.hashResultSize];
        memcpy(PowValue.hashResult, PowValue.origin, PowValue.hashResultSize);
        switch (PowValue.algorithmType)
        {
        case 1:
            while (true)
            {
                byte sha256[32];
                Utils::Sha256(PowValue.hashResult, PowValue.hashResultSize, sha256);
                if (!memcmp(PowValue.cp, sha256, 32))
                    break;
                PowValue.cnt++;
                BigInteger::AddOne(PowValue.hashResult, PowValue.hashResultSize);
            }
            break;
        case 2:
            throw "hash func not support sm3";
            break;
        default:
            break;
        }
        break;
    default:
        throw "not support algorithm";
        return 0;
    }
    PowValue.cost = (int)(Utils::CurrentTimeMillis() - currentTimeMillis);
    PowValue.hasHashResult = 1;
    PowValue.cnt = 1;

    PackPowValue(&Pack, &PowValue);
    return Pack.Pack(0x0547);
}

FUNC(548)
{
    ::TlvPack Pack(bin, len);

    long st = Utils::CurrentTimeMillis();

    PowValue PowValue;
    PowValue.version = 1;
    PowValue.checkType = 2;
    PowValue.algorithmType = 1;
    PowValue.hasHashResult = 1;

    PowValue.baseCount = 10;
    PowValue.filling[0] = 0;
    PowValue.filling[1] = 0;

    PowValue.originSize = 128;
    PowValue.origin = Utils::GetRandomBin(PowValue.originSize);

    PowValue.cpSize = 32;
    PowValue.cp = Utils::Sha256(PowValue.origin, 128);

    ::Pack _Pack;
    _Pack.SetByte(PowValue.version);
    _Pack.SetByte(PowValue.checkType);
    _Pack.SetByte(PowValue.algorithmType);
    _Pack.SetByte(PowValue.hasHashResult);
    _Pack.SetShort(PowValue.baseCount);
    for (int fill : PowValue.filling)
        _Pack.SetByte(fill);
    _Pack.SetShort(PowValue.originSize);
    _Pack.SetBin(PowValue.origin, PowValue.originSize);
    _Pack.SetShort(PowValue.cpSize);
    _Pack.SetBin(PowValue.cp, PowValue.cpSize);

    PowValue.sm = _Pack.GetAll();
    PowValue.sm3Size = _Pack.Length();

    PowValue.hashResultSize = 128;
    PowValue.hashResult = new byte[PowValue.hashResultSize];
    memcpy(PowValue.hashResult, PowValue.origin, PowValue.originSize);

    PowValue.cnt = 10000;
    PowValue.cost = (int)(Utils::CurrentTimeMillis() - st);

    PackPowValue(&Pack, &PowValue);
    return Pack.Pack(0X0548);
}

FUNC(553)
{
    // qSec.getFeKitAttach(u.w, "", "0x810", "0x12")
    // Todo
    ::TlvPack Pack(bin, len);
    // Pack.SetBin();
    return Pack.Pack(0X0553);
}

#undef FUNC