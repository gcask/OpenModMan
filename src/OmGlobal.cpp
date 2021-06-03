/*
  This file is part of Open Mod Manager.

  Open Mod Manager is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Open Mod Manager is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Open Mod Manager. If not, see <http://www.gnu.org/licenses/>.
*/

#include "OmGlobal.h"
#include <regex>

///
/// Currently not used
///
//#define UINT64_C(n) n##ull

/// \brief CRC64 table.
///
/// CRC64 table for fast CRC calculation.
///
static const uint64_t crc64_tab[256] = {
    UINT64_C(0x0000000000000000), UINT64_C(0x7ad870c830358979),
    UINT64_C(0xf5b0e190606b12f2), UINT64_C(0x8f689158505e9b8b),
    UINT64_C(0xc038e5739841b68f), UINT64_C(0xbae095bba8743ff6),
    UINT64_C(0x358804e3f82aa47d), UINT64_C(0x4f50742bc81f2d04),
    UINT64_C(0xab28ecb46814fe75), UINT64_C(0xd1f09c7c5821770c),
    UINT64_C(0x5e980d24087fec87), UINT64_C(0x24407dec384a65fe),
    UINT64_C(0x6b1009c7f05548fa), UINT64_C(0x11c8790fc060c183),
    UINT64_C(0x9ea0e857903e5a08), UINT64_C(0xe478989fa00bd371),
    UINT64_C(0x7d08ff3b88be6f81), UINT64_C(0x07d08ff3b88be6f8),
    UINT64_C(0x88b81eabe8d57d73), UINT64_C(0xf2606e63d8e0f40a),
    UINT64_C(0xbd301a4810ffd90e), UINT64_C(0xc7e86a8020ca5077),
    UINT64_C(0x4880fbd87094cbfc), UINT64_C(0x32588b1040a14285),
    UINT64_C(0xd620138fe0aa91f4), UINT64_C(0xacf86347d09f188d),
    UINT64_C(0x2390f21f80c18306), UINT64_C(0x594882d7b0f40a7f),
    UINT64_C(0x1618f6fc78eb277b), UINT64_C(0x6cc0863448deae02),
    UINT64_C(0xe3a8176c18803589), UINT64_C(0x997067a428b5bcf0),
    UINT64_C(0xfa11fe77117cdf02), UINT64_C(0x80c98ebf2149567b),
    UINT64_C(0x0fa11fe77117cdf0), UINT64_C(0x75796f2f41224489),
    UINT64_C(0x3a291b04893d698d), UINT64_C(0x40f16bccb908e0f4),
    UINT64_C(0xcf99fa94e9567b7f), UINT64_C(0xb5418a5cd963f206),
    UINT64_C(0x513912c379682177), UINT64_C(0x2be1620b495da80e),
    UINT64_C(0xa489f35319033385), UINT64_C(0xde51839b2936bafc),
    UINT64_C(0x9101f7b0e12997f8), UINT64_C(0xebd98778d11c1e81),
    UINT64_C(0x64b116208142850a), UINT64_C(0x1e6966e8b1770c73),
    UINT64_C(0x8719014c99c2b083), UINT64_C(0xfdc17184a9f739fa),
    UINT64_C(0x72a9e0dcf9a9a271), UINT64_C(0x08719014c99c2b08),
    UINT64_C(0x4721e43f0183060c), UINT64_C(0x3df994f731b68f75),
    UINT64_C(0xb29105af61e814fe), UINT64_C(0xc849756751dd9d87),
    UINT64_C(0x2c31edf8f1d64ef6), UINT64_C(0x56e99d30c1e3c78f),
    UINT64_C(0xd9810c6891bd5c04), UINT64_C(0xa3597ca0a188d57d),
    UINT64_C(0xec09088b6997f879), UINT64_C(0x96d1784359a27100),
    UINT64_C(0x19b9e91b09fcea8b), UINT64_C(0x636199d339c963f2),
    UINT64_C(0xdf7adabd7a6e2d6f), UINT64_C(0xa5a2aa754a5ba416),
    UINT64_C(0x2aca3b2d1a053f9d), UINT64_C(0x50124be52a30b6e4),
    UINT64_C(0x1f423fcee22f9be0), UINT64_C(0x659a4f06d21a1299),
    UINT64_C(0xeaf2de5e82448912), UINT64_C(0x902aae96b271006b),
    UINT64_C(0x74523609127ad31a), UINT64_C(0x0e8a46c1224f5a63),
    UINT64_C(0x81e2d7997211c1e8), UINT64_C(0xfb3aa75142244891),
    UINT64_C(0xb46ad37a8a3b6595), UINT64_C(0xceb2a3b2ba0eecec),
    UINT64_C(0x41da32eaea507767), UINT64_C(0x3b024222da65fe1e),
    UINT64_C(0xa2722586f2d042ee), UINT64_C(0xd8aa554ec2e5cb97),
    UINT64_C(0x57c2c41692bb501c), UINT64_C(0x2d1ab4dea28ed965),
    UINT64_C(0x624ac0f56a91f461), UINT64_C(0x1892b03d5aa47d18),
    UINT64_C(0x97fa21650afae693), UINT64_C(0xed2251ad3acf6fea),
    UINT64_C(0x095ac9329ac4bc9b), UINT64_C(0x7382b9faaaf135e2),
    UINT64_C(0xfcea28a2faafae69), UINT64_C(0x8632586aca9a2710),
    UINT64_C(0xc9622c4102850a14), UINT64_C(0xb3ba5c8932b0836d),
    UINT64_C(0x3cd2cdd162ee18e6), UINT64_C(0x460abd1952db919f),
    UINT64_C(0x256b24ca6b12f26d), UINT64_C(0x5fb354025b277b14),
    UINT64_C(0xd0dbc55a0b79e09f), UINT64_C(0xaa03b5923b4c69e6),
    UINT64_C(0xe553c1b9f35344e2), UINT64_C(0x9f8bb171c366cd9b),
    UINT64_C(0x10e3202993385610), UINT64_C(0x6a3b50e1a30ddf69),
    UINT64_C(0x8e43c87e03060c18), UINT64_C(0xf49bb8b633338561),
    UINT64_C(0x7bf329ee636d1eea), UINT64_C(0x012b592653589793),
    UINT64_C(0x4e7b2d0d9b47ba97), UINT64_C(0x34a35dc5ab7233ee),
    UINT64_C(0xbbcbcc9dfb2ca865), UINT64_C(0xc113bc55cb19211c),
    UINT64_C(0x5863dbf1e3ac9dec), UINT64_C(0x22bbab39d3991495),
    UINT64_C(0xadd33a6183c78f1e), UINT64_C(0xd70b4aa9b3f20667),
    UINT64_C(0x985b3e827bed2b63), UINT64_C(0xe2834e4a4bd8a21a),
    UINT64_C(0x6debdf121b863991), UINT64_C(0x1733afda2bb3b0e8),
    UINT64_C(0xf34b37458bb86399), UINT64_C(0x8993478dbb8deae0),
    UINT64_C(0x06fbd6d5ebd3716b), UINT64_C(0x7c23a61ddbe6f812),
    UINT64_C(0x3373d23613f9d516), UINT64_C(0x49aba2fe23cc5c6f),
    UINT64_C(0xc6c333a67392c7e4), UINT64_C(0xbc1b436e43a74e9d),
    UINT64_C(0x95ac9329ac4bc9b5), UINT64_C(0xef74e3e19c7e40cc),
    UINT64_C(0x601c72b9cc20db47), UINT64_C(0x1ac40271fc15523e),
    UINT64_C(0x5594765a340a7f3a), UINT64_C(0x2f4c0692043ff643),
    UINT64_C(0xa02497ca54616dc8), UINT64_C(0xdafce7026454e4b1),
    UINT64_C(0x3e847f9dc45f37c0), UINT64_C(0x445c0f55f46abeb9),
    UINT64_C(0xcb349e0da4342532), UINT64_C(0xb1eceec59401ac4b),
    UINT64_C(0xfebc9aee5c1e814f), UINT64_C(0x8464ea266c2b0836),
    UINT64_C(0x0b0c7b7e3c7593bd), UINT64_C(0x71d40bb60c401ac4),
    UINT64_C(0xe8a46c1224f5a634), UINT64_C(0x927c1cda14c02f4d),
    UINT64_C(0x1d148d82449eb4c6), UINT64_C(0x67ccfd4a74ab3dbf),
    UINT64_C(0x289c8961bcb410bb), UINT64_C(0x5244f9a98c8199c2),
    UINT64_C(0xdd2c68f1dcdf0249), UINT64_C(0xa7f41839ecea8b30),
    UINT64_C(0x438c80a64ce15841), UINT64_C(0x3954f06e7cd4d138),
    UINT64_C(0xb63c61362c8a4ab3), UINT64_C(0xcce411fe1cbfc3ca),
    UINT64_C(0x83b465d5d4a0eece), UINT64_C(0xf96c151de49567b7),
    UINT64_C(0x76048445b4cbfc3c), UINT64_C(0x0cdcf48d84fe7545),
    UINT64_C(0x6fbd6d5ebd3716b7), UINT64_C(0x15651d968d029fce),
    UINT64_C(0x9a0d8ccedd5c0445), UINT64_C(0xe0d5fc06ed698d3c),
    UINT64_C(0xaf85882d2576a038), UINT64_C(0xd55df8e515432941),
    UINT64_C(0x5a3569bd451db2ca), UINT64_C(0x20ed197575283bb3),
    UINT64_C(0xc49581ead523e8c2), UINT64_C(0xbe4df122e51661bb),
    UINT64_C(0x3125607ab548fa30), UINT64_C(0x4bfd10b2857d7349),
    UINT64_C(0x04ad64994d625e4d), UINT64_C(0x7e7514517d57d734),
    UINT64_C(0xf11d85092d094cbf), UINT64_C(0x8bc5f5c11d3cc5c6),
    UINT64_C(0x12b5926535897936), UINT64_C(0x686de2ad05bcf04f),
    UINT64_C(0xe70573f555e26bc4), UINT64_C(0x9ddd033d65d7e2bd),
    UINT64_C(0xd28d7716adc8cfb9), UINT64_C(0xa85507de9dfd46c0),
    UINT64_C(0x273d9686cda3dd4b), UINT64_C(0x5de5e64efd965432),
    UINT64_C(0xb99d7ed15d9d8743), UINT64_C(0xc3450e196da80e3a),
    UINT64_C(0x4c2d9f413df695b1), UINT64_C(0x36f5ef890dc31cc8),
    UINT64_C(0x79a59ba2c5dc31cc), UINT64_C(0x037deb6af5e9b8b5),
    UINT64_C(0x8c157a32a5b7233e), UINT64_C(0xf6cd0afa9582aa47),
    UINT64_C(0x4ad64994d625e4da), UINT64_C(0x300e395ce6106da3),
    UINT64_C(0xbf66a804b64ef628), UINT64_C(0xc5bed8cc867b7f51),
    UINT64_C(0x8aeeace74e645255), UINT64_C(0xf036dc2f7e51db2c),
    UINT64_C(0x7f5e4d772e0f40a7), UINT64_C(0x05863dbf1e3ac9de),
    UINT64_C(0xe1fea520be311aaf), UINT64_C(0x9b26d5e88e0493d6),
    UINT64_C(0x144e44b0de5a085d), UINT64_C(0x6e963478ee6f8124),
    UINT64_C(0x21c640532670ac20), UINT64_C(0x5b1e309b16452559),
    UINT64_C(0xd476a1c3461bbed2), UINT64_C(0xaeaed10b762e37ab),
    UINT64_C(0x37deb6af5e9b8b5b), UINT64_C(0x4d06c6676eae0222),
    UINT64_C(0xc26e573f3ef099a9), UINT64_C(0xb8b627f70ec510d0),
    UINT64_C(0xf7e653dcc6da3dd4), UINT64_C(0x8d3e2314f6efb4ad),
    UINT64_C(0x0256b24ca6b12f26), UINT64_C(0x788ec2849684a65f),
    UINT64_C(0x9cf65a1b368f752e), UINT64_C(0xe62e2ad306bafc57),
    UINT64_C(0x6946bb8b56e467dc), UINT64_C(0x139ecb4366d1eea5),
    UINT64_C(0x5ccebf68aecec3a1), UINT64_C(0x2616cfa09efb4ad8),
    UINT64_C(0xa97e5ef8cea5d153), UINT64_C(0xd3a62e30fe90582a),
    UINT64_C(0xb0c7b7e3c7593bd8), UINT64_C(0xca1fc72bf76cb2a1),
    UINT64_C(0x45775673a732292a), UINT64_C(0x3faf26bb9707a053),
    UINT64_C(0x70ff52905f188d57), UINT64_C(0x0a2722586f2d042e),
    UINT64_C(0x854fb3003f739fa5), UINT64_C(0xff97c3c80f4616dc),
    UINT64_C(0x1bef5b57af4dc5ad), UINT64_C(0x61372b9f9f784cd4),
    UINT64_C(0xee5fbac7cf26d75f), UINT64_C(0x9487ca0fff135e26),
    UINT64_C(0xdbd7be24370c7322), UINT64_C(0xa10fceec0739fa5b),
    UINT64_C(0x2e675fb4576761d0), UINT64_C(0x54bf2f7c6752e8a9),
    UINT64_C(0xcdcf48d84fe75459), UINT64_C(0xb71738107fd2dd20),
    UINT64_C(0x387fa9482f8c46ab), UINT64_C(0x42a7d9801fb9cfd2),
    UINT64_C(0x0df7adabd7a6e2d6), UINT64_C(0x772fdd63e7936baf),
    UINT64_C(0xf8474c3bb7cdf024), UINT64_C(0x829f3cf387f8795d),
    UINT64_C(0x66e7a46c27f3aa2c), UINT64_C(0x1c3fd4a417c62355),
    UINT64_C(0x935745fc4798b8de), UINT64_C(0xe98f353477ad31a7),
    UINT64_C(0xa6df411fbfb21ca3), UINT64_C(0xdc0731d78f8795da),
    UINT64_C(0x536fa08fdfd90e51), UINT64_C(0x29b7d047efec8728),
};


/// \brief Calculate CRC64.
///
/// Calculates and return the CRC64 value of the specified data chunk.
///
/// \param[in]  crc     : seed value for computation. 0 or (u64)~0 for a new CRC
///                       calculation, or the previous crc64 value if computing
///                       incrementally.
/// \param[in]  s       :  pointer to data chunk over which CRC64 is run.
/// \param[in]  l       :  data chunk size in bytes.
///
/// \return Converted 64 bits integer value.
///
static uint64_t __CRC64(uint64_t crc, const unsigned char *s, uint64_t l)
{
  uint64_t j;
  for (j = 0; j < l; ++j) {
      uint8_t byte = s[j];
      crc = crc64_tab[(uint8_t)crc ^ byte] ^ (crc >> 8);
  }
  return crc;
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
uint64_t Om_getCRC64(const void* data, size_t size)
{
  return __CRC64(0, (unsigned char*)data, size);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
uint64_t Om_getCRC64(const wstring& str)
{
  return __CRC64(0, (unsigned char*)str.c_str(), str.size()*sizeof(wchar_t));
}

#include "thirdparty/xxhash/xxh3.h"

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
uint64_t Om_getXXHash3(const void* data, size_t size)
{
  return static_cast<uint64_t>(XXH3_64bits(data, size)); // XXH64_hash_t
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
uint64_t Om_getXXHash3(const wstring& str)
{
  return static_cast<uint64_t>(XXH3_64bits((unsigned char*)str.c_str(), str.size()*sizeof(wchar_t))); // XXH64_hash_t
}


static const wchar_t __b64_chars[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
wstring Om_toBase64(const uint8_t* data, size_t size)
{
  wstring b64;

  unsigned p, n, t, rem = size % 3;  //< remaining bytes after per-triplet division

  // main block, per triplets
  for(p = 0, n = size - rem; p < n; ) {
    t = (data[p++] << 16) | (data[p++] << 8) | data[p++];
    b64 += __b64_chars[0x3F & (t >> 18)];
    b64 += __b64_chars[0x3F & (t >> 12)];
    b64 += __b64_chars[0x3F & (t >>  6)];
    b64 += __b64_chars[0x3F & (t)];
  }

  // remaining bytes + padding
  if(rem != 0) {
    t = (data[p++] << 16);
    if(rem > 1) t |= (data[p] << 8);
    b64 += __b64_chars[0x3F & (t >> 18)];
    b64 += __b64_chars[0x3F & (t >> 12)];
    if(rem > 1) {
      b64 += __b64_chars[0x3F & (t >>  6)];
    } else {
      b64 += L"=";
    }
    b64 += L"=";
  }

  return b64;
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///

void Om_toBase64(wstring& b64, const uint8_t* data, size_t size)
{
  unsigned p, n, t, rem = size % 3;  //< remaining bytes after per-triplet division

  // main block, per triplets
  for(p = 0, n = size - rem; p < n; ) {
    t = (data[p++] << 16) | (data[p++] << 8) | data[p++];
    b64 += __b64_chars[0x3F & (t >> 18)];
    b64 += __b64_chars[0x3F & (t >> 12)];
    b64 += __b64_chars[0x3F & (t >>  6)];
    b64 += __b64_chars[0x3F & (t)];
  }

  // remaining bytes + padding
  if(rem != 0) {
    t = (data[p++] << 16);
    if(rem > 1) t |= (data[p] << 8);
    b64 += __b64_chars[0x3F & (t >> 18)];
    b64 += __b64_chars[0x3F & (t >> 12)];
    if(rem > 1) {
      b64 += __b64_chars[0x3F & (t >>  6)];
    } else {
      b64 += L"=";
    }
    b64 += L"=";
  }
}

#include <ctime>
time_t __time_rtime;
struct tm* __time_ltime;

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_getTime(int *t_sec, int *t_min, int *t_hour)
{
  time(&__time_rtime);
  __time_ltime = localtime(&__time_rtime);

  if(t_sec != nullptr) *t_sec = __time_ltime->tm_sec;
  if(t_min != nullptr) *t_min = __time_ltime->tm_min;
  if(t_hour != nullptr) *t_hour = __time_ltime->tm_hour;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_getDate(int *t_day, int *t_mon, int *t_year)
{
  time(&__time_rtime);
  __time_ltime = localtime(&__time_rtime);

  if(t_day != nullptr) *t_day = __time_ltime->tm_mday;
  if(t_mon != nullptr) *t_mon = __time_ltime->tm_mon;
  if(t_year != nullptr) *t_year = __time_ltime->tm_year;
}


#include <random>

static mt19937                             __rnd_generator(time(0));
static uniform_int_distribution<uint8_t>   __rnd_uint8dist(0, 255);

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_getRandBytes(uint8_t* dest, size_t size)
{
  for(size_t i = 0; i < size; ++i) {
    dest[i] = __rnd_uint8dist(__rnd_generator);
  }
}


/// \brief Hexadecimal digits
///
/// Static translation string to convert integer value to hexadecimal digit.
///
static const wchar_t __hex_digit[] = L"0123456789abcdef";


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
wstring Om_genUUID()
{
  uint8_t uuid[16];

  // Generate random bytes sequence
  for(unsigned i = 0; i < 16; ++i) {
    uuid[i] = __rnd_uint8dist(__rnd_generator);
  }

  uuid[6] = (uuid[6] & 0x0F) | 0x40; //< Set version 4
  uuid[8] = (uuid[8] & 0x3F) | 0x80; //< Set DCE variant

  // Create formated UUID string from random sequence
  wstring str(L"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx");

  unsigned u = 0;
  unsigned c = 0;

  while(c < 36) {

    if(str[c] == L'-') {
      ++c; continue;
    }

    str[c] = __hex_digit[uuid[u] >>  4]; ++c;
    str[c] = __hex_digit[uuid[u] & 0xf]; ++c;

    ++u;
  }

  return str;
}

/// \brief UTF-8 decode
///
/// Convert the given multibyte string into wide char string assuming UTF-8
/// encoding.
///
/// This function si no longer used in current implementation but conserved
/// here in cas this is needed.
///
/// \param[in]  wstr    : Wide char string to receive conversion result.
/// \param[out] utf8    : Pointer to null-terminated Multibyte string to convert.
///
inline static void __utf8_decode(wstring& wstr, const char* utf8)
{
  wstr.clear();
  wstr.reserve(strlen(utf8));

  uint32_t u;
  unsigned char c;
  size_t p = 0;

  while(utf8[p] != 0) {

    c = utf8[p];

    if(c <= 0x7F) {
      u = c;
    } else if(c <= 0xBF) {
      u = (u << 6) | (c & 0x3F);
    } else if(c <= 0xdf) {
      u = c & 0x1F;
    } else if(c <= 0xEF) {
      u = c & 0x0F;
    } else {
      u = c & 0x07;
    }

    ++p;

    if(((utf8[p] & 0xC0) != 0x80) && (u <= 0x10FFFF)) {
      if(u > 0xFFFF)  {
        wstr.push_back(static_cast<wchar_t>(0xD800 + (u >> 10)));
        wstr.push_back(static_cast<wchar_t>(0xDC00 + (u & 0x03FF)));
      } else if(u < 0xD800 || u >= 0xE000) {
        wstr.push_back(static_cast<wchar_t>(u));
      }
    }
  }
}


/// \brief UTF-8 encode
///
/// Convert the given wide char string to multibyte string using UTF-8
/// encoding.
///
/// This function si no longer used in current implementation but conserved
/// here in cas this is needed.
///
/// \param[out] utf8    : Multibyte string to receive conversion result.
/// \param[in]  wstr    : Wide char string to convert.
///
inline static void __utf8_encode(string& utf8, const wstring& wstr)
{
  utf8.clear();
  utf8.reserve(wstr.size() * 4);

  char16_t c;
  uint32_t u;

  for(size_t i = 0; i < wstr.size(); ++i) {

    c = wstr[i];

    if(c >= 0xD800 && c <= 0xDBFF) {
      u = ((c - 0xD800) << 10) + 0x10000;
      continue; //< get next char
    } else if(c >= 0xDC00 && c <= 0xDFFF) {
      u |= c - 0xDC00;
    } else {
      u = c;
    }

    if(u <= 0x7F) {
      utf8.push_back(static_cast<char>(u));
    }else if(u <= 0x7FF) {
      utf8.push_back(static_cast<char>(0xC0 | ((u >> 6) & 0x1F)));
      utf8.push_back(static_cast<char>(0x80 | ( u       & 0x3F)));
    } else if(u <= 0xFFFF) {
      utf8.push_back(static_cast<char>(0xE0 | ((u >> 12) & 0x0F)));
      utf8.push_back(static_cast<char>(0x80 | ((u >>  6) & 0x3F)));
      utf8.push_back(static_cast<char>(0x80 | ( u        & 0x3F)));
    } else {
      utf8.push_back(static_cast<char>(0xF0 | ((u >> 18) & 0x07)));
      utf8.push_back(static_cast<char>(0x80 | ((u >> 12) & 0x3F)));
      utf8.push_back(static_cast<char>(0x80 | ((u >>  6) & 0x3F)));
      utf8.push_back(static_cast<char>(0x80 | ( u        & 0x3F)));
    }

    u = 0;
  }
}

/// \brief Multibyte Decode
///
/// Static inlined function to convert the given multibyte string into wide
/// char string assuming the specified encoding.
///
/// This function use the WinAPI MultiByteToWideChar implementation witch is
/// currently the known fastest way.
///
/// \param[in]  cp      : Code page to use in performing the conversion.
/// \param[in]  wstr    : Wide char string to receive conversion result.
/// \param[out] utf8    : Pointer to null-terminated Multibyte string to convert.
///
inline static size_t __multibyte_decode(UINT cp, wstring& wstr, const char* utf8)
{
  int n = MultiByteToWideChar(cp, 0, utf8, -1, nullptr, 0);

  if(n > 0) {
    wstr.resize(n - 1);
    // NOTICE: here bellow, the string object is used as C char buffer, in
    // theory this is not allowed since std::string is not required to store
    // its contents contiguously in memory.
    //
    // HOWEVER, in practice, there is no know situation where std::string does
    // not store its content contiguous, so, this should work anyway.
    //
    // If some problem emerge from this function, change this implementation for
    // a more regular approach.
    return static_cast<size_t>(MultiByteToWideChar(cp, 0, utf8, -1, &wstr[0], n));
  }

  return 0;
}


/// \brief Multibyte encode
///
/// Static inlined function to convert the given wide char string to multibyte
/// string using the specified encoding.
///
/// This function use the WinAPI MultiByteToWideChar implementation witch is
/// currently the known fastest way.
///
/// \param[in]  cp      : Code page to use in performing the conversion.
/// \param[out] utf8    : Multibyte string to receive conversion result.
/// \param[in]  wstr    : Wide char string to convert.
///
inline static size_t __multibyte_encode(UINT cp, string& utf8, const wchar_t* wstr)
{
  BOOL pBool;
  int n = WideCharToMultiByte(cp, 0, wstr, -1, nullptr, 0, nullptr, &pBool);
  if(n > 0) {
    utf8.resize(n - 1);
    // NOTICE: here bellow, the string object is used as C char buffer, in
    // theory this is not allowed since std::string is not required to store
    // its contents contiguously in memory.
    //
    // HOWEVER, in practice, there is no know situation where std::string does
    // not store its content contiguous, so, this should work anyway.
    //
    // If some problem emerge from this function, change this implementation for
    // a more regular approach.
    return static_cast<size_t>(WideCharToMultiByte(cp, 0, wstr, -1, &utf8[0], n, nullptr, &pBool));
  }
  return 0;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
wstring Om_fromUtf8(const char* utf8)
{
  wstring result;
  __multibyte_decode(CP_UTF8, result, utf8);
  return result;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
size_t Om_fromUtf8(wstring& wstr, const char* utf8)
{
  return __multibyte_decode(CP_UTF8, wstr, utf8);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
size_t Om_toUtf8(char* utf8, size_t len, const wstring& wstr)
{
  // The WinAPI implementation is the fastest one at this time
  BOOL pBool;
  int n = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, utf8, len, nullptr, &pBool);
  return (n > 0) ? static_cast<size_t>(n) : 0;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
string Om_toUtf8(const wstring& wstr)
{
  string result;
  __multibyte_encode(CP_UTF8, result, wstr.c_str());
  return result;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
size_t Om_toUtf8(string& utf8, const wstring& wstr)
{
  return __multibyte_encode(CP_UTF8, utf8, wstr.c_str());
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
size_t Om_toAnsiCp(char* ansi, size_t len, const wstring& wstr)
{
  // The WinAPI implementation is the fastest one at this time
  BOOL pBool;
  int n = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, ansi, len, nullptr, &pBool);
  return (n > 0) ? static_cast<size_t>(n) : 0;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
size_t Om_toAnsiCp(string& ansi, const wstring& wstr)
{
  return __multibyte_encode(CP_ACP, ansi, wstr.c_str());
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
size_t Om_fromAnsiCp(wstring& wstr, const char* ansi)
{
  return __multibyte_decode(CP_ACP, wstr, ansi);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
size_t Om_toZipCDR(char* cdr, size_t len, const wstring& wstr)
{
  // The WinAPI implementation is the fastest one at this time
  BOOL pBool;
  int n = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, cdr, len, nullptr, &pBool);

  for(size_t i = 0; cdr[i] != 0; ++i) {
    if(cdr[i] == '\\') cdr[i] = '/';
  }

  return (n > 0) ? static_cast<size_t>(n) : 0;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
size_t Om_toZipCDR(string& cdr, const wstring& wstr)
{
  __multibyte_encode(CP_UTF8, cdr, wstr.c_str());

  for(size_t i = 0; i < cdr.size(); ++i) {
    if(cdr[i] == '\\') cdr[i] = '/';
  }

  return cdr.size();
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
size_t Om_fromZipCDR(wstring& wstr, const char* cdr)
{
  __multibyte_decode(CP_UTF8, wstr, cdr);

  for(size_t i = 0; i < wstr.size(); ++i) {
    if(wstr[i] == L'/') wstr[i] = L'\\';
  }

  return wstr.size();
}


/// \brief Sort comparison function
///
/// Comparison callback function for std::sort() to sort strings in
/// alphabetical order
///
/// \param[in]  a   : left string to compare.
/// \param[in]  b   : right string to compare.
///
/// \return True if a is "before" b, false otherwise
///
static bool __sortStrings_Func(const wstring& a, const wstring& b)
{
  // get size of the shorter string to compare
  size_t l = a.size() > b.size() ? b.size() : a.size();

  // test for ASCII value greater than the other
  for(unsigned i = 0; i < l; ++i) {
    if(towupper(a[i]) != towupper(b[i])) {
      if(towupper(a[i]) < towupper(b[i])) {
        return true;
      } else {
        return false;
      }
    }
  }

  // if function does not returned at this stage, this mean strings
  // are equals in the tested portion, so the longer one is after
  if(a.size() < b.size())
    return true;

  // if the two strings are strictly equals, then, we don't care
  return false;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_sortStrings(vector<wstring>* strings)
{
  std::sort(strings->begin(), strings->end(), __sortStrings_Func);
}


/// \brief Illegal characters
///
/// List of forbidden characters to test validity of file name or path.
///
static const wchar_t __illegal_chr[] = L"/*?\"<>|\\";


/// \brief URL Regex pattern
///
/// Regular expression pattern to check whether string is a valid URL according RFC 3986.
///
static const std::regex __url_reg(R"(^(https?:\/\/)([\da-z\.-]+)(:[\d]+)?([\/\w\.%-]*)(\?[\w%-=&]+)?)"); // work never


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_isValidUrl(const string& url)
{
  std::match_results<const char*> matches;

  if(std::regex_match(url.c_str(), matches, __url_reg)) {

    // matches :
    // 0) full match (almost never happen)
    // 1) http(s)://
    // 2) xxx.www.domain.tld
    // 3) :1234
    // 4) /folder/file.ext
    // 5) ?x=1&y=2...

    // search for minimum required matches to have a full valid URL
    if(matches[1].length() != 0) { //< http(s)://
      if(matches[2].length() != 0) { //< xxx.www.domain.tld
        return (matches[4].length() != 0); //< /folder/file.ext
      }
    }
  }

  return false;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_isValidName(const wchar_t* name)
{
  if(!wcslen(name))
    return false;

  for(unsigned i = 0; i < 8; ++i) // forbids all including back-slash
    if(wcschr(name, __illegal_chr[i]))
      return false;

  return true;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_isValidName(const wstring& name)
{
  if(name.empty())
    return false;

  for(unsigned i = 0; i < 8; ++i) // forbids all including back-slash
    if(name.find_first_of(__illegal_chr[i]) != wstring::npos)
      return false;

  return true;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_isValidPath(const wchar_t* path)
{
  if(!wcslen(path))
    return false;

  // check for illegal characters in path
  for(unsigned i = 0; i < 7; ++i)  // excluding back-slash
    if(wcschr(path, __illegal_chr[i]))
      return false;

  return true;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_isValidPath(const wstring& path)
{
  if(path.empty())
    return false;

  for(unsigned i = 0; i < 7; ++i)  // excluding back-slash
    if(path.find_first_of(__illegal_chr[i]) != wstring::npos)
      return false;

  return true;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
wstring Om_sizeString(size_t bytes, bool octet)
{
  wchar_t swp_buf[64];
  wchar_t unit = (octet) ? 'o' : 'B';

  double fbytes;
  wstring result;

  if(bytes < 1024) { // 1 Ko
    if(octet) {
      swprintf(swp_buf, 64, L"%d Octet(s)", bytes);
    } else {
      swprintf(swp_buf, 64, L"%d Byte(s)", bytes);
    }
  } else if(bytes < 1048576) { // 1 Mo
    fbytes = (double)bytes / 1024.0;
    swprintf(swp_buf, 64, L"%.1f Ki%lc", fbytes, unit);
  } else if(bytes < 1073741824) { // 1 Go
    fbytes = (double)bytes / 1048576.0;
    swprintf(swp_buf, 64, L"%.1f Mi%lc", fbytes, unit);
  } else {
    fbytes = (double)bytes / 1073741824.0;
    swprintf(swp_buf, 64, L"%.1f Gi%lc", fbytes, unit);
  }

  result = swp_buf;
  return result;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_isVersionStr(const wstring& str)
{
  unsigned n = 0;
  unsigned j = 0;

  for(unsigned i = 0; i < str.size(); ++i) {

    if(str[i] > 47 && str[i] < 58) { // 0123456789

      if(j < 15) {
        ++j;
      } else {
        return false;
      }

    } else {

      if(str[i] == L'.') {
        if(j > 0) {
          n++; j = 0;
        }
      }

      if(n > 2)
        break;
    }
  }

  return (j > 0 || n > 0);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_parsePkgIdent(wstring& name, wstring& vers, const wstring& filename, bool isfile, bool us2spc)
{
  wstring ident;

  // Get the proper part of the full filename
  if(isfile) {
    // get file name without file extension
    ident = Om_getNamePart(filename);
  } else {
    // this extract the folder name from the full path
    ident = Om_getFilePart(filename);
  }

  // parse raw name to get display name and potential version
  bool has_version = false;
  // we search a version part in the name, this must be the letter V preceded
  // by a common separator, like space, minus or underscore character, followed
  // by a number
  size_t v_pos = ident.find_last_of(L"vV");
  if(v_pos > 0) {
    // verify the V letter is preceded by a common separator
    wchar_t wc = ident[v_pos - 1];
    if(wc == L' ' || wc == L'_' || wc == L'-') {
      // verify the V letter is followed by a number
      wc = ident[v_pos + 1];
      if(wc > 0x29 && wc < 0x40) { // 0123456789
        // get the substring from v char to the end of string
        vers = ident.substr(v_pos+1, -1);
        has_version = Om_isVersionStr(vers);
      }
    }
  }

  if(has_version) {
    // we extract the substring from the beginning to the version substring
    name = ident.substr(0, v_pos);
  } else {
    vers.clear();
    name = ident;
  }

  // replace all underscores by spaces
  if(us2spc) {
    std::replace(name.begin(), name.end(), L'_', L' ');
  }

  return has_version;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
int Om_dirDeleteRecursive(const wstring& path)
{
  wchar_t* path_buf = nullptr;

  try {
    path_buf = new wchar_t[path.size()+2];
  } catch(std::bad_alloc& ba) {
    std::cerr << "Om_dirDeleteRecursive:: bad_alloc : " << ba.what();
    return 1;
  }

  wcscpy(path_buf, path.c_str());
  path_buf[path.size()+1] = 0; // the buffer must end with double null character

  SHFILEOPSTRUCTW fop = {};
  fop.pFrom = path_buf;
  fop.wFunc = FO_DELETE;
  fop.fFlags = FOF_NO_UI;

  int result = SHFileOperationW(&fop);

  delete [] path_buf;

  return result;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_isFileZip(const wstring& path) {

  // Microsoft functions are ugly (this is not new) but they are proven to be
  // the most efficient for file I/O... maybe because they are directly sticked
  // to the kernel at low level...
  HANDLE hFile = CreateFileW(path.c_str(),
                             GENERIC_READ,
                             FILE_SHARE_READ,
                             nullptr,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             nullptr);

  if(hFile == INVALID_HANDLE_VALUE)
    return false;

  // We have to seek over the file until we found something else
  // that zero, because a zip file can begin with a blank space before the
  // signature...

  char buf[1024]; // <- our read buffer
  unsigned* sign; // <- our future 4 bytes signature
  DWORD r = 0; //< count of bytes read

  do {
    SetFilePointer(hFile, r, nullptr, FILE_CURRENT);
    ReadFile(hFile, &buf, 1024, &r, nullptr);
    for(unsigned i = 0; i < r; ++i) {
      // check for something else than zero
      if(buf[i] != 0) {
        // we got something, we don't go further we close the file
        // and check the result.
        CloseHandle(hFile);
        sign = (unsigned*)&buf[i]; //< cast our buffer in unsigned pointer
        return ( *sign == 0x04034b50 );
      }
    }
  } while(r == 1024);

  CloseHandle(hFile);
  return false; // PKWARE Zip file signature
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_lsDir(vector<wstring>* ls, const wstring& orig, bool absolute)
{
  wstring item;

  wstring srch(orig); srch += L"\\*";
  WIN32_FIND_DATAW fd;
  HANDLE hnd = FindFirstFileW(srch.c_str(), &fd);
  if(hnd != INVALID_HANDLE_VALUE) {
    ls->clear();
    do {
      if(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) {
        // skip this and parent folder
        if(!wcscmp(fd.cFileName, L".")) continue;
        if(!wcscmp(fd.cFileName, L"..")) continue;

        if(absolute) {
          item = orig; item += L"\\"; item += fd.cFileName;
          ls->push_back(item);
        } else {
          ls->push_back(fd.cFileName);
        }
      }
    } while(FindNextFileW(hnd, &fd));
  }
  FindClose(hnd);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_lsFile(vector<wstring>* ls, const wstring& orig, bool absolute)
{
  wstring item;

  wstring srch(orig); srch += L"\\*";
  WIN32_FIND_DATAW fd;
  HANDLE hnd = FindFirstFileW(srch.c_str(), &fd);
  if(hnd != INVALID_HANDLE_VALUE) {
    ls->clear();
    do {
      if(!(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {
        if(absolute) {
          item = orig; item += L"\\"; item += fd.cFileName;
          ls->push_back(item);
        } else {
          ls->push_back(fd.cFileName);
        }
      }
    } while(FindNextFileW(hnd, &fd));
  }
  FindClose(hnd);
}


/// \brief List files recursively
///
/// This is the private function used to list files recursively.
///
/// \param[out] ls      : Pointer to array of wstring to be filled with result.
/// \param[in]  orig    : Path where to list items from.
/// \param[in]  from    : Path to prepend to result to obtain the item full
///                       path from the beginning of the tree exploration.
///
static void __lsFile_Recurse(vector<wstring>* ls, const wstring& orig, const wstring& from)
{
  wstring item;
  wstring root;

  wstring srch(orig); srch += L"\\*";
  WIN32_FIND_DATAW fd;
  HANDLE hnd = FindFirstFileW(srch.c_str(), &fd);
  if(hnd != INVALID_HANDLE_VALUE) {
    do {
      // skip this and parent folder
      if(!wcscmp(fd.cFileName, L".")) continue;
      if(!wcscmp(fd.cFileName, L"..")) continue;

      if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        item = from; item += L"\\"; item += fd.cFileName;
        root = orig; root += L"\\"; root += fd.cFileName;
        // go deep in tree
        __lsFile_Recurse(ls, root, item);

      } else {
        item = from; item += L"\\"; item += fd.cFileName;
        ls->push_back(item);
      }
    } while(FindNextFileW(hnd, &fd));
  }
  FindClose(hnd);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_lsFileRecursive(vector<wstring>* ls, const wstring& origin, bool absolute)
{
  ls->clear();
  if(absolute) {
    __lsFile_Recurse(ls, origin.c_str(), origin.c_str());
  } else {
    __lsFile_Recurse(ls, origin.c_str(), L"");
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_lsFileFiltered(vector<wstring>* ls, const wstring& orig, const wstring& filter, bool absolute)
{
  wstring item;
  wstring root;

  wstring srch(orig);
  srch += L"\\";
  srch += filter;

  WIN32_FIND_DATAW fd;
  HANDLE hnd = FindFirstFileW(srch.c_str(), &fd);
  if(hnd != INVALID_HANDLE_VALUE) {
    ls->clear();
    do {
      if(!(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {
        if(absolute) {
          item = orig; item += L"\\"; item += fd.cFileName;
          ls->push_back(item);
        } else {
          ls->push_back(fd.cFileName);
        }
      }
    } while(FindNextFileW(hnd, &fd));
  }
  FindClose(hnd);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_lsAll(vector<wstring>* ls, const wstring& orig, bool absolute)
{
  wstring item;

  wstring srch(orig); srch += L"\\*";
  WIN32_FIND_DATAW fd;
  HANDLE hnd = FindFirstFileW(srch.c_str(), &fd);
  if(hnd != INVALID_HANDLE_VALUE) {
    ls->clear();
    do {
      // skip this and parent folder
      if(!wcscmp(fd.cFileName, L".")) continue;
      if(!wcscmp(fd.cFileName, L"..")) continue;

      if(absolute) {
        item = orig; item += L"\\"; item += fd.cFileName;
        ls->push_back(item);
      } else {
        ls->push_back(fd.cFileName);
      }
    } while(FindNextFileW(hnd, &fd));
  }
  FindClose(hnd);
}


/// \brief List files and folders recursively
///
/// This is the private function used to list files and folder recursively.
///
/// \param[out] ls      : Pointer to array of wstring to be filled with result.
/// \param[in]  orig    : Path where to list items from.
/// \param[in]  from    : Path to prepend to result to obtain the item full
///                       path from the beginning of the tree exploration.
///
static void __lsAll_Recurse(vector<wstring>* ls, const wstring& orig, const wstring& from)
{
  wstring item;
  wstring root;

  wstring srch(orig); srch += L"\\*";
  WIN32_FIND_DATAW fd;
  HANDLE hnd = FindFirstFileW(srch.c_str(), &fd);
  if(hnd != INVALID_HANDLE_VALUE) {
    do {
      // skip this and parent folder
      if(!wcscmp(fd.cFileName, L".")) continue;
      if(!wcscmp(fd.cFileName, L"..")) continue;

      if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        item = from; item += L"\\"; item += fd.cFileName;
        root = orig; root += L"\\"; root += fd.cFileName;
        ls->push_back(item);

        // go deep in tree
        __lsAll_Recurse(ls, root, item);

      } else {
        item = from; item += L"\\"; item += fd.cFileName;
        ls->push_back(item);
      }
    } while(FindNextFileW(hnd, &fd));
  }
  FindClose(hnd);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_lsAllRecursive(vector<wstring>* ls, const wstring& origin, bool absolute)
{
  ls->clear();
  if(absolute) {
    __lsAll_Recurse(ls, origin.c_str(), origin.c_str());
  } else {
    __lsAll_Recurse(ls, origin.c_str(), L"");
  }
}


/// \brief Compute folder size recursively
///
/// Private function to calculate the total size of folder content including
/// all it sub-folders. The function act recursively.
///
/// \param[out] size    : Pointer to variable to be incremented as result.
/// \param[in]  orig    : Path of folder to get total size (start of recursive
///                       exploration).
///
void __folderSize(size_t* size, const wstring& orig)
{
  wstring root;

  wstring srch(orig); srch += L"\\*";
  WIN32_FIND_DATAW fd;
  HANDLE hnd = FindFirstFileW(srch.c_str(), &fd);
  if(hnd != INVALID_HANDLE_VALUE) {
    do {
      // skip this and parent folder
      if(!wcscmp(fd.cFileName, L".")) continue;
      if(!wcscmp(fd.cFileName, L"..")) continue;

      if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        root = orig; root += L"\\"; root += fd.cFileName;

        // go deep in tree
        __folderSize(size, root);

      } else {
        root = orig; root += L"\\"; root += fd.cFileName;
        HANDLE hFile = CreateFileW(root.c_str(),
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   nullptr,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   nullptr);
        *size += GetFileSize(hFile, nullptr);
        CloseHandle(hFile);
      }
    } while(FindNextFileW(hnd, &fd));
  }
  FindClose(hnd);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
size_t Om_itemSize(const wstring& path)
{
  size_t ret;

  if(Om_isFile(path)) {
    HANDLE hFile = CreateFileW(path.c_str(),
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               nullptr,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);
    ret = GetFileSize(hFile, nullptr);
    CloseHandle(hFile);
  } else {
    ret = 0;
    __folderSize(&ret, path);
  }

  return ret;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
time_t Om_itemTime(const wstring& path)
{
  time_t ret;
  FILETIME fTime;

  if(Om_isFile(path)) {
    HANDLE hFile = CreateFileW(path.c_str(),
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               nullptr,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);

    GetFileTime(hFile, nullptr, nullptr, &fTime);
    CloseHandle(hFile);

    ULARGE_INTEGER ull;
    ull.LowPart = fTime.dwLowDateTime;
    ull.HighPart = fTime.dwHighDateTime;
    ret = ull.QuadPart / 10000000ULL - 11644473600ULL;

  } else {
    ret = 0;
  }

  return ret;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
int Om_moveToTrash(const wstring& path)
{
  wchar_t* path_buf = nullptr;

  try {
    path_buf = new wchar_t[path.size()+2];
  } catch(std::bad_alloc& ba) {
    std::cerr << "Om_moveToTrash:: bad_alloc :" << ba.what();
    return 1;
  }

  wcscpy(path_buf, path.c_str());
  path_buf[path.size()+1] = 0;

  SHFILEOPSTRUCTW fop = {};
  fop.pFrom = path_buf;
  fop.wFunc = FO_DELETE;
  fop.fFlags = FOF_NO_UI|FOF_ALLOWUNDO;

  int result = SHFileOperationW(&fop);

  delete [] path_buf;

  return result;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_checkAccess(const wstring& path, unsigned mask)
{
  //return __checkAccess(path, mask);

  // Thanks to this article for giving some clues :
  // http://blog.aaronballman.com/2011/08/how-to-check-access-rights/

  // STEP 1 - retrieve the "security descriptor" (i.e owner, group, access
  // rights, etc. ) of the specified file or folder.
  SECURITY_DESCRIPTOR* pSd;
  DWORD sdSize;
  // here is the mask for file permission informations we want to retrieve it
  // seem to be the minimum required for an access check request
  DWORD sdMask =  OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION
                | DACL_SECURITY_INFORMATION;
  // first call to get required SECURITY_DESCRIPTOR size
  GetFileSecurityW(path.c_str(), sdMask, nullptr, 0, &sdSize);
  // allocate new SECURITY_DESCRIPTOR of the proper size
  pSd = reinterpret_cast<SECURITY_DESCRIPTOR*>(new char[sdSize+1]);
  // second call to get SECURITY_DESCRIPTOR data
  if(!GetFileSecurityW(path.c_str(), sdMask, pSd, sdSize, &sdSize)) {
    delete pSd; return false;
  }

  // STEP 2 - creates a "security token" of the current application process
  //to be checked against the file or folder "security descriptor"
  DWORD daMask =  TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_DUPLICATE
                | STANDARD_RIGHTS_READ;
  HANDLE hTokenProc = nullptr;
  if(!OpenProcessToken(GetCurrentProcess(), daMask, &hTokenProc)) {
    delete pSd; return false;
  }
  // the current process token is a "primary" one (don't know what that mean)
  // so we need to duplicate it to transform it into a standard "user" token by
  // impersonate it...
  HANDLE hTokenUser = nullptr;
  if(!DuplicateToken(hTokenProc, SecurityImpersonation, &hTokenUser)) {
    CloseHandle(hTokenProc); delete pSd;
    return false;
  }

  // STEP 3 - Finally check if "security token" have the requested
  // "mask" access to the "security descriptor" of the specified file
  // or folder

  // the GENERIC_MAPPING seem to be never used in most common scenarios,
  // we set it here because the parameter is mandatory.
  GENERIC_MAPPING gm = {GENERIC_READ,GENERIC_WRITE,GENERIC_EXECUTE,GENERIC_ALL};
  PRIVILEGE_SET ps = {};
  DWORD psSize = sizeof(PRIVILEGE_SET);
  DWORD allowed = 0;      //< mask of allowed access
  BOOL  status = false;   //< access status according supplied GENERIC_MAPPING
  AccessCheck(pSd, hTokenUser, mask, &gm, &ps, &psSize, &allowed, &status);

  if(!status) {
    std::wcout << L"__checkAccess: denied, allowed access (mask): \n";
    AccessCheck(pSd, hTokenUser, FILE_READ_DATA, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  FILE_READ_DATA + LIST_DIRECTORY\n";
    AccessCheck(pSd, hTokenUser, FILE_WRITE_DATA, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  FILE_WRITE_DATA + ADD_FILE\n";
    AccessCheck(pSd, hTokenUser, FILE_APPEND_DATA, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  FILE_APPEND_DATA + ADD_SUBDIRECTORY\n";
    AccessCheck(pSd, hTokenUser, FILE_READ_EA, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  FILE_READ_EA\n";
    AccessCheck(pSd, hTokenUser, FILE_WRITE_EA, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  FILE_WRITE_EA\n";
    AccessCheck(pSd, hTokenUser, FILE_EXECUTE, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  FILE_EXECUTE + TRAVERSE\n";
    AccessCheck(pSd, hTokenUser, FILE_DELETE_CHILD, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  FILE_DELETE_CHILD\n";
    AccessCheck(pSd, hTokenUser, FILE_READ_ATTRIBUTES, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  FILE_READ_ATTRIBUTES\n";
    AccessCheck(pSd, hTokenUser, FILE_WRITE_ATTRIBUTES, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  FILE_WRITE_ATTRIBUTES\n";
    AccessCheck(pSd, hTokenUser, DELETE, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  DELETE\n";
    AccessCheck(pSd, hTokenUser, READ_CONTROL, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  READ_CONTROL\n";
    AccessCheck(pSd, hTokenUser, WRITE_DAC, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  WRITE_DAC\n";
    AccessCheck(pSd, hTokenUser, WRITE_OWNER, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  WRITE_OWNER\n";
    AccessCheck(pSd, hTokenUser, SYNCHRONIZE, &gm, &ps, &psSize, &allowed, &status);
    std::wcout << ((status) ? L"[x]" : L"[ ]") << L"  SYNCHRONIZE\n";
    status = 0;
  }

  CloseHandle(hTokenProc);
  CloseHandle(hTokenUser);
  delete pSd;

  return status;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_dialogBoxErr(HWND hWnd, const wstring& header, const wstring& detail)
{
  wstring message = header + L"\n\n"; message += detail;
  MessageBoxW(hWnd, message.c_str(), OMM_APP_NAME, MB_OK|MB_ICONERROR);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_dialogBoxWarn(HWND hWnd, const wstring& header, const wstring& detail)
{
  wstring message = header + L"\n\n"; message += detail;
  MessageBoxW(hWnd, message.c_str(), OMM_APP_NAME, MB_OK|MB_ICONWARNING);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void Om_dialogBoxInfo(HWND hWnd, const wstring& header, const wstring& detail)
{
  wstring message = header + L"\n\n"; message += detail;
  MessageBoxW(hWnd, message.c_str(), OMM_APP_NAME, MB_OK|MB_ICONINFORMATION);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_dialogBoxQuerry(HWND hWnd, const wstring& header, const wstring& detail)
{
  wstring message = header + L"\n\n"; message += detail;
  return (IDYES == MessageBoxW(hWnd, message.c_str(), OMM_APP_NAME, MB_YESNO|MB_ICONQUESTION));
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_dialogBoxQuerryWarn(HWND hWnd, const wstring& header, const wstring& detail)
{
  wstring message = header + L"\n\n"; message += detail;
  return (IDYES == MessageBoxW(hWnd, message.c_str(), OMM_APP_NAME, MB_YESNO|MB_ICONWARNING));
}


/// \brief Browse dialog callback.
///
/// Callback function for folder browse dialog window. It is used like a
/// WindowProc to select a default start folder when dialog window is
/// initialized.
///
INT CALLBACK __dialogBrowseDir_Proc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  if(uMsg == BFFM_INITIALIZED) { //< Brother dialog windows is initialized
    SendMessageW(hWnd, BFFM_SETSELECTION, false, lpData);  //< set the selected folder
  }
  return 0;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_dialogBrowseDir(wstring& result, HWND hWnd, const wchar_t* title, const wstring& start, bool captive)
{
  BROWSEINFOW bI = {};
  bI.hwndOwner = hWnd;
  bI.lpszTitle = title;
  bI.ulFlags = BIF_USENEWUI|BIF_RETURNONLYFSDIRS|BIF_VALIDATE;

  if(captive) {
    // this is the standard easy way to use SHBrowseForFolderW, this will
    // set the "start" path as the root of browsing, so the user cannot go up
    // to parent folder
    if(start.size()) {
      PIDLIST_ABSOLUTE pIdl = nullptr;
      SHParseDisplayName(start.c_str(), nullptr, &pIdl, 0, nullptr); //< convert path string to LPITEMIDLIST
      bI.pidlRoot = pIdl;
    }
  } else {
      // this is the advanced way to use SHBrowseForFolderW, here we use a
      // callback function to handle the dialog window initialization, the "start"
      // path object will be passed as lParam to the callback with the
      // BFFM_INITIALIZED message.
    if(start.size()) {
      PIDLIST_ABSOLUTE pIdl = nullptr;
      SHParseDisplayName(start.c_str(), nullptr, &pIdl, 0, nullptr); //< convert path string to LPITEMIDLIST
      bI.lpfn = __dialogBrowseDir_Proc;
      bI.lParam = reinterpret_cast<LPARAM>(pIdl);
    }
  }

  bool suceess = false;

  LPITEMIDLIST pIdl;
  if((pIdl = SHBrowseForFolderW(&bI)) != nullptr) {

    wchar_t psz_path[OMM_MAX_PATH];

    psz_path[0] = 0;
    if(SHGetPathFromIDListEx(pIdl, psz_path, OMM_MAX_PATH, GPFIDL_DEFAULT)) {
      result = psz_path;
      suceess = true;
    }
  }

  CoTaskMemFree(pIdl);

  return suceess;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_dialogOpenFile(wstring& result, HWND hWnd, const wchar_t* title, const wchar_t* filter, const wstring& start)
{
  wchar_t str_file[OMM_MAX_PATH];

  OPENFILENAMEW ofn = {};
  ofn.lStructSize = sizeof(OPENFILENAMEW);

  ofn.hwndOwner = hWnd;
  ofn.lpstrFilter = filter; //L"Mod archive (*.zip)\0*.ZIP;\0";

  ofn.lpstrFile = str_file;
  ofn.lpstrFile[0] = L'\0';
  ofn.nMaxFile = OMM_MAX_PATH;

  ofn.lpstrInitialDir = start.c_str();

  ofn.lpstrTitle = title;
  ofn.Flags = OFN_EXPLORER|OFN_NONETWORKBUTTON|OFN_NOTESTFILECREATE;

  if(GetOpenFileNameW(&ofn)) {
    result = str_file;
    return true;
  }

  return false;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_dialogSaveFile(wstring& result, HWND hWnd, const wchar_t* title, const wchar_t* filter, const wstring& start)
{
  wchar_t str_file[OMM_MAX_PATH];
  swprintf(str_file, OMM_MAX_PATH, L"%ls", result.c_str());

  OPENFILENAMEW ofn = {};
  ofn.lStructSize = sizeof(OPENFILENAMEW);

  ofn.hwndOwner = hWnd;
  ofn.lpstrFilter = filter;

  ofn.lpstrFile = str_file;
  ofn.nMaxFile = OMM_MAX_PATH;

  ofn.lpstrInitialDir = start.c_str();

  ofn.lpstrTitle = title;
  ofn.Flags = OFN_EXPLORER|OFN_NONETWORKBUTTON|OFN_NOTESTFILECREATE;

  if(GetSaveFileNameW(&ofn)) {
    result = str_file;
    return true;
  }

  return false;
}


/// \brief Load plan text.
///
/// Loads content of the specified file as plain-text into the given
/// string object.
///
/// \param[in] text    : String to receive loaded text data.
/// \param[in] path    : Path to text file to be loaded.
///
/// \return Count of bytes read.
///
inline static size_t __load_plaintxt(string& txt, const wchar_t* path)
{
  txt.clear();

  HANDLE hFile = CreateFileW( path, GENERIC_READ, 0, nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, nullptr);

  if(hFile == INVALID_HANDLE_VALUE)
    return 0;

  DWORD rb;
  size_t rt = 0;
  char cbuf[1080];

  while(ReadFile(hFile, cbuf, 1024, &rb, nullptr)) {

    if(rb == 0)
      break;

    rt += rb;

    cbuf[rb] = '\0';

    txt.append(cbuf);
  }

  CloseHandle(hFile);

  return rt;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
string Om_loadPlainText(const wstring& path)
{
  string result;
  __load_plaintxt(result, path.c_str());
  return result;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
size_t Om_loadPlainText(string& text, const wstring& path)
{
  return __load_plaintxt(text, path.c_str());
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
#include "thirdparty/jpeg/jpeglib.h"
#include "thirdparty/png/png.h"
#include "thirdparty/gif/gif_lib.h"
#include "thirdparty/gif/quantize.c"

/* we make sure structures are packed to be properly aligned with
 read buffer */
#pragma pack(1)
/// \brief BMP info header
///
/// Structure for BMP file info header
struct OMM_BITMAPINFOHEADER {
  uint32_t  size;           ///< size of the structure
  uint32_t  width;          ///< image width
  uint32_t  height;         ///< image height
  uint16_t  planes;         ///< bit planes
  int16_t   bpp;            ///< bit per pixel
  uint32_t  compression;    ///< compression
  uint32_t  sizeimage;      ///< size of the image
  int32_t   xppm;           ///< pixels per meter X
  int32_t   yppm;           ///< pixels per meter Y
  uint32_t  clrused;        ///< colors used
  uint32_t  clrimportant;   ///< important colors
}; // 40 bytes
/// \brief BMP base header
///
/// Structure for BMP file base header
struct OMM_BITMAPHEADER {
  uint8_t   signature[2];   ///< BM magic word
  uint32_t  size;           ///< size of the whole .bmp file
  uint16_t  reserved1;      ///< must be 0
  uint16_t  reserved2;      ///< must be 0
  uint32_t  offbits;        ///< where bitmap data begins
}; // 14 bytes
#pragma pack()

/// BMP file specific signature / magic number
static const unsigned char __sign_bmp[] = "BM";
/// JPG file specific signature / magic number
static const unsigned char __sign_jpg[] = {0xFF, 0xD8, 0xFF};
/// PNG file specific signature / magic number
static const unsigned char __sign_png[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
/// GIF file specific signature / magic number
static const unsigned char __sign_gif[] = "GIF89a";

/// \brief compare buffer to known images signatures
///
/// Check whether the given buffer matches any known file
/// signature or magic number.
///
/// \param[in]  buff  : Buffer to compare known signatures with
///
/// \return found image type or 0
///
inline static unsigned __image_sign_matches(const uint8_t* buff)
{
  // Test BMP signature
  if(0 == memcmp(buff, __sign_bmp, 2)) return OMM_IMAGE_TYPE_BMP;
  // Test JPG signature
  if(0 == memcmp(buff, __sign_jpg, 3)) return OMM_IMAGE_TYPE_JPG;
  // Test PNG signature
  if(0 == memcmp(buff, __sign_png, 8)) return OMM_IMAGE_TYPE_PNG;
  // Test GIF signature
  if(0 == memcmp(buff, __sign_gif, 6)) return OMM_IMAGE_TYPE_GIF;

  return 0;
}

/// \brief Custom GIF reader
///
/// Custom read function for GIF library to read a file pointer.
///
/// \param[in]  gif     Decoder structure pointer.
/// \param[in]  dst     Destination buffer.
/// \param[in]  len     Length of data that should be read.
///
/// \return  The number of bytes that were read
///
static int __gif_read_file_fn(GifFileType* gif, uint8_t* dst, int len)
{
  return fread(dst, 1, len, reinterpret_cast<FILE*>(gif->UserData));
}

/// \brief Custom GIF reader
///
/// Custom read function for GIF library to read memory buffer.
///
/// \param[in]  gif     Decoder structure pointer.
/// \param[in]  dst     Destination buffer.
/// \param[in]  len     Length of data that should be read.
///
/// \return  The number of bytes that were read
///
static int __gif_read_buff_fn(GifFileType* gif, uint8_t* dst, int len)
{
  memcpy(dst, gif->UserData, len);
  return len;
}

/// \brief Custom GIF writer
///
/// Custom read function for GIF library to write a file pointer.
///
/// \param[in]  gif     Decoder structure pointer.
/// \param[in]  src     Source buffer.
/// \param[in]  len     Length of data that should be read.
///
/// \return  The number of length that were read
///
static int __gif_write_file_fn(GifFileType* gif, const uint8_t* src, int len)
{
  return fwrite(src, 1, len, reinterpret_cast<FILE*>(gif->UserData));
}

/// \brief Custom GIF write struct
///
/// Custom structure for custom GIF write routine.
///
struct __gif_write_st {
  uint8_t*  dst_data;
  size_t    dst_size;
  size_t    dst_seek;
};

/// \brief Custom GIF writer
///
/// Custom write function for GIF library to encode to memory.
///
/// \param[in]  gif     Decoder structure pointer.
/// \param[in]  src     Source buffer.
/// \param[in]  len     Length of data that should be read.
///
/// \return  The number of length that were read
///
static int __gif_write_buff_fn(GifFileType* gif, const uint8_t* src, int len)
{
  __gif_write_st* write_st = reinterpret_cast<__gif_write_st*>(gif->UserData);
  write_st->dst_size += len;
  if(write_st->dst_data) {
    write_st->dst_data = reinterpret_cast<uint8_t*>(realloc(write_st->dst_data, write_st->dst_size));
  } else {
    write_st->dst_data = reinterpret_cast<uint8_t*>(malloc(write_st->dst_size));
  }
  memcpy(write_st->dst_data + write_st->dst_seek, src, len);
  write_st->dst_seek += len;
  return len;
}

/// \brief Decode GIF.
///
/// Common function to decode GIF using the given GIF decoder structure.
///
/// \param[in]  gif_dec : GIF decoder structure pointer.
/// \param[out] out_rgb : Output image RGB(A) data, pointer to pointer to be allocated.
/// \param[out] out_w   : Output image width
/// \param[out] out_h   : Output image height
/// \param[out] out_c   : Output image color component count.
/// \param[in]  flip_y  : Load image for bottom-left origin usage (upside down)
///
/// \return True if operation succeed, false otherwise
///
static bool __gif_decode_common(void* gif_dec, uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, bool flip_y)
{
  // Retrieve GIF decoder struct
  int error;
  GifFileType* gif = reinterpret_cast<GifFileType*>(gif_dec);

  // Load GIF content
  if(DGifSlurp(gif) == GIF_ERROR) {
    DGifCloseFile(gif, &error);
    return false;
  }

  // Get image list, we care only about the first one
  SavedImage* images = gif->SavedImages;

  // Get image informations
  unsigned w = images[0].ImageDesc.Width;
  unsigned h = images[0].ImageDesc.Height;

  // Pointer to color table
  ColorMapObject* table;

  // check whether we got a local color table (may never happen)
  if(images[0].ImageDesc.ColorMap) {
    table = images[0].ImageDesc.ColorMap;
  } else {
    table = gif->SColorMap;
  }

  // define some useful sizes
  size_t row_bytes = w * 3;
  size_t tot_bytes = h * row_bytes;

  // allocate new buffer for RGB data
  uint8_t* rgb;
  try {
    rgb = new uint8_t[tot_bytes];
  } catch(const std::bad_alloc&) {
    DGifCloseFile(gif, &error);
    return false;
  }

  // get GIF index list
  uint8_t* sp = static_cast<uint8_t*>(images[0].RasterBits);

  // destination pointer
  uint8_t* dp;

  // here we go to translate indexed color to RGB
  for(unsigned y = 0; y < h; ++y) {

    dp = (flip_y) ? rgb + (row_bytes * ((h -1) - y)) : rgb + (y * row_bytes);

    for(unsigned x = 0; x < w; ++x) {
      dp[0] = table->Colors[*sp].Red;
      dp[1] = table->Colors[*sp].Green;
      dp[2] = table->Colors[*sp].Blue;
      dp += 3; sp++;
    }
  }

  // free decoder
  DGifCloseFile(gif, &error);

  // assign output values
  (*out_rgb) = rgb; (*out_w) = w; (*out_h) = h; (*out_c) = 3;

  return true;
}

/// \brief Encode GIF.
///
/// Common function to encode GIF using the given GIF encoder structure.
///
/// \param[in]  gif_enc : GIF encoder structure pointer.
/// \param[in]  in_rgb  : Input image RGB(A) data to encode.
/// \param[in]  in_w    : Input image width.
/// \param[in]  in_h    : Input image height.
/// \param[in]  in_c    : Input image color component count.
///
/// \return True if operation succeed, false otherwise
///
static bool __gif_encode_common(void* gif_enc, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  // Retrieve GIF encoder struct
  int error;
  GifFileType* gif = reinterpret_cast<GifFileType*>(gif_enc);

  // compute image indices array size
  size_t idx_bytes = in_w * in_h;

  // create red, green and blue array for quantizing
  uint8_t* in_r;
  try {
    in_r = new uint8_t[idx_bytes];
  } catch(const std::bad_alloc&) {
    return false;
  }
  uint8_t* in_g;
  try {
    in_g = new uint8_t[idx_bytes];
  } catch(const std::bad_alloc&) {
    delete [] in_r;
    return false;
  }
  uint8_t* in_b;
  try {
    in_b = new uint8_t[idx_bytes];
  } catch(const std::bad_alloc&) {
    delete [] in_r; delete [] in_g;
    return false;
  }

  const uint8_t* sp;

  unsigned i = 0;

  for(unsigned y = 0; y < in_h; ++y) {

    sp = in_rgb + (y * in_h * in_c);

    for(unsigned x = 0; x < in_w; ++x) {
      in_r[i] = sp[0];
      in_g[i] = sp[1];
      in_b[i] = sp[2];
      sp += in_c; ++i;
    }
  }

  // allocate new buffer to receive color indices
  uint8_t* indices;
  try {
    indices = new uint8_t[idx_bytes];
  } catch(const std::bad_alloc&) {
    delete [] in_r; delete [] in_g; delete [] in_b;
    return false;
  }

  // allocate new color map of 256 colors
  GifColorType* table;
  try {
    table = new GifColorType[256];
  } catch(const std::bad_alloc&) {
    delete [] in_r; delete [] in_g; delete [] in_b;
    delete [] indices;
    return false;
  }

  // quantize image
  int table_size = 256;
  if(GIF_OK != GifQuantizeBuffer(in_w, in_h, &table_size, in_r, in_g, in_b, indices, table)) {
    delete [] in_r; delete [] in_g; delete [] in_b;
    delete [] indices; delete [] table;
    return false;
  }

  // we do not need color array anymore
  delete [] in_r; delete [] in_g; delete [] in_b;

  // set GIF global parameters
  gif->SWidth = in_w;
  gif->SHeight = in_h;
  gif->SColorResolution = 8;
  gif->SBackGroundColor = 0;
  gif->SColorMap = GifMakeMapObject(table_size, table); //< global color table

  // set image parameters
  SavedImage image;
  image.ImageDesc.Left = 0;
  image.ImageDesc.Top = 0;
  image.ImageDesc.Width = in_w;
  image.ImageDesc.Height = in_h;
  image.ImageDesc.Interlace = false;
  image.ImageDesc.ColorMap = nullptr; //< no local color table
  image.RasterBits = indices; //< our color indices
  image.ExtensionBlockCount = 0;
  image.ExtensionBlocks = nullptr;

  // add image to gif encoder
  GifMakeSavedImage(gif, &image);

  // encode GIF
  if(GIF_OK != EGifSpew(gif)) {
    delete [] indices; delete [] table;
    EGifCloseFile(gif, &error);
    return false;
  }

  // free allocated data
  delete [] indices; delete [] table;

  return true;
}

/// \brief Decode GIF.
///
/// Decode GIF data from file pointer.
///
/// \param[out] out_rgb : Output image RGB(A) data, pointer to pointer to be allocated.
/// \param[out] out_w   : Output image width
/// \param[out] out_h   : Output image height
/// \param[out] out_c   : Output image color component count.
/// \param[in]  in_file : Input file pointer to read data from.
/// \param[in]  flip_y  : Load image for bottom-left origin usage (upside down)
///
/// \return True if operation succeed, false otherwise
///
static bool __gif_decode(uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, FILE* in_file, bool flip_y)
{
  int error;
  GifFileType* gif;

  // make sure we start at beginning
  fseek(in_file, 0, SEEK_SET);

  // Define custom read function and load GIF header
  gif = DGifOpen(in_file, __gif_read_file_fn, &error);
  if(gif == nullptr)
    return false;

  // Decode GIF data
  return __gif_decode_common(gif, out_rgb, out_w, out_h, out_c, flip_y);
}

/// \brief Decode GIF.
///
/// Decode GIF data from buffer in memory.
///
/// \param[out] out_rgb : Output image RGB(A) data, pointer to pointer to be allocated.
/// \param[out] out_w   : Output image width
/// \param[out] out_h   : Output image height
/// \param[out] out_c   : Output image color component count.
/// \param[in]  in_data : Input GIF data to decode.
/// \param[in]  flip_y  : Load image for bottom-left origin usage (upside down)
///
/// \return True if operation succeed, false otherwise
///
static bool __gif_decode(uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, uint8_t* in_data, bool flip_y)
{
  int error;
  GifFileType* gif;

  // Define custom read function and load GIF header
  gif = DGifOpen(in_data, __gif_read_buff_fn, &error);
  if(gif == nullptr)
    return false;

  // Decode GIF data
  return __gif_decode_common(gif, out_rgb, out_w, out_h, out_c, flip_y);
}

/// \brief Encode GIF.
///
/// Encode GIF data to file pointer.
///
/// \param[out] out_file  : File pointer to write to.
/// \param[in]  in_rgb    : Input image RGB(A) data to encode.
/// \param[in]  in_w      : Input image width.
/// \param[in]  in_h      : Input image height.
/// \param[in]  in_c      : Input image color component count, either 3 or 4.
///
/// \return True if operation succeed, false otherwise
///
static bool __gif_encode(FILE* out_file, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  int error;
  GifFileType* gif;

  // Define custom read function and load GIF header
  gif = EGifOpen(out_file, __gif_write_file_fn, &error);
  if(gif == nullptr)
    return false;

  // Encode RGB to GIF data
  return __gif_encode_common(gif, in_rgb, in_w, in_h, in_c);
}

/// \brief Encode GIF.
///
/// Encode GIF data to buffer in memory.
///
/// \param[out] out_data  : Output GIF data, pointer to pointer to be allocated.
/// \param[out] out_size  : Output GIF data size in bytes.
/// \param[in]  in_rgb    : Input image RGB(A) data to encode.
/// \param[in]  in_w      : Input image width.
/// \param[in]  in_h      : Input image height.
/// \param[in]  in_c      : Input image color component count, either 3 or 4.
///
/// \return True if operation succeed, false otherwise
///
static bool __gif_encode(uint8_t** out_data, size_t* out_size, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  int error;
  GifFileType* gif;

  // custom write structure
  __gif_write_st write_st;
  write_st.dst_data = nullptr;
  write_st.dst_size = 0;
  write_st.dst_seek = 0;

  // Define custom read function and load GIF header
  gif = EGifOpen(&write_st, __gif_write_buff_fn, &error);
  if(gif == nullptr)
    return false;

  // Encode RGB to GIF data
  if(!__gif_encode_common(gif, in_rgb, in_w, in_h, in_c))
    return false;

  // assign output values
  (*out_data) = write_st.dst_data;
  (*out_size) = write_st.dst_size;

  return 1;
}

/// \brief Decode BMP.
///
/// Decode BMP data from file pointer.
///
/// \param[out] out_rgb : Output image RGB(A) data, pointer to pointer to be allocated.
/// \param[out] out_w   : Output image width
/// \param[out] out_h   : Output image height
/// \param[out] out_c   : Output image color component count.
/// \param[in]  in_file : Input file pointer to read data from.
/// \param[in]  flip_y  : Load image for bottom-left origin usage (upside down)
///
/// \return True if operation succeed, false otherwise
///
static bool __bmp_decode(uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, FILE* in_file, bool flip_y)
{
  // make sure we start at begining
  fseek(in_file, 0, SEEK_SET);

  // BMP headers structures
  OMM_BITMAPHEADER bmp_head;
  OMM_BITMAPINFOHEADER bmp_info;
  // read base header
  if(fread(&bmp_head, 1, 14, in_file) < 14) return false;
  // read info header
  if(fread(&bmp_info, 1, 40, in_file) < 40) return false;
  // check BM signature
  if(0 != memcmp(bmp_head.signature, "BM", 2)) return false;

  // we support only 24 or 32 bpp
  if(bmp_info.bpp < 24)
    return false;
  // get BMP image parameters
  unsigned w = bmp_info.width;
  unsigned h = bmp_info.height;
  unsigned c = bmp_info.bpp / 8; // channel count

  // define some useful sizes
  size_t row_bytes = w * c;
  size_t tot_bytes = h * row_bytes;
  // allocate new buffer to receive rgb data
  uint8_t* rgb;
  try {
    rgb = new uint8_t[tot_bytes];
  } catch(const std::bad_alloc&) {
    return false;
  }

  // seek to bitmap data location and read
  fseek(in_file, bmp_head.offbits, SEEK_SET);

  // BMP data is natively stored upside down
  if(flip_y) {
    // read all data at once from
    if(fread(rgb, 1, tot_bytes, in_file) != tot_bytes) {
      delete[] rgb; return false;
    }
  } else {
    // read rows in reverse order
    unsigned hmax = (h - 1);
    for(unsigned y = 0; y < h; ++y) {
      if(fread(rgb + (row_bytes * (hmax - y)), 1, row_bytes, in_file) != row_bytes) {
        delete[] rgb; return false;
      }
    }
  }

  // finally swap components order BGR to RGB
  for(unsigned i = 0; i < tot_bytes; i += c)
    rgb[i  ] ^= rgb[i+2] ^= rgb[i  ] ^= rgb[i+2]; //< BGR => RGB

  // assign output values
  (*out_rgb) = rgb; (*out_w) = w; (*out_h) = h; (*out_c) = c;

  return true;
}

/// \brief Decode BMP.
///
/// Decode BMP data from buffer in memory.
///
/// \param[out] out_rgb : Output image RGB(A) data, pointer to pointer to be allocated.
/// \param[out] out_w   : Output image width
/// \param[out] out_h   : Output image height
/// \param[out] out_c   : Output image color component count.
/// \param[in]  in_data : Input BMP data to decode.
/// \param[in]  flip_y  : Load image for bottom-left origin usage (upside down)
///
/// \return True if operation succeed, false otherwise
///
static int __bmp_decode(uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, const uint8_t* in_data, bool flip_y)
{
  // pointer to input data
  const uint8_t* in_ptr = in_data;

  // BMP headers structures
  OMM_BITMAPHEADER bmp_head;
  OMM_BITMAPINFOHEADER bmp_info;
  // get base header
  memcpy(&bmp_head, in_ptr, 14); in_ptr += 14;
  // get info header
  memcpy(&bmp_info, in_ptr, 40); in_ptr += 40;
  // check BM signature
  if(0 != memcmp(bmp_head.signature, "BM", 2)) return false;

  // we support only 24 or 32 bpp
  if(bmp_info.bpp < 24)
    return false;
  // get BMP image parameters
  unsigned w = bmp_info.width;
  unsigned h = bmp_info.height;
  unsigned c = bmp_info.bpp / 8; // channel count

  // define some useful sizes
  size_t row_bytes = w * c;
  size_t tot_bytes = h * row_bytes;
  // allocate new buffer to receive rgb data
  uint8_t* rgb;
  try {
    rgb = new uint8_t[tot_bytes];
  } catch(const std::bad_alloc&) {
    return false;
  }

  // seek to bitmap data location
  in_ptr = in_data + bmp_head.offbits;

  // BMP data is natively stored upside down
  if(flip_y) {
    // read all data at once from
    memcpy(rgb, in_ptr, tot_bytes); in_ptr += tot_bytes;
  } else {
    // read rows in reverse order
    unsigned hmax = (h - 1);
    for(unsigned y = 0; y < h; ++y) {
      memcpy(rgb + (row_bytes * (hmax - y)), in_ptr, row_bytes);
      in_ptr += row_bytes;
    }
  }

  // finally swap components order BGR to RGB
  for(unsigned i = 0; i < tot_bytes; i += c)
    rgb[i  ] ^= rgb[i+2] ^= rgb[i  ] ^= rgb[i+2]; //< BGR => RGB

  // assign output values
  (*out_rgb) = rgb; (*out_w) = w; (*out_h) = h; (*out_c) = c;

  return true;
}

/// \brief Encode BMP.
///
/// Encode BMP data to file pointer.
///
/// \param[out] out_file  : File pointer to write to.
/// \param[in]  in_rgb    : Input image RGB(A) data to encode.
/// \param[in]  in_w      : Input image width.
/// \param[in]  in_h      : Input image height.
/// \param[in]  in_c      : Input image color component count, either 3 or 4.
///
/// \return True if operation succeed, false otherwise
///
static bool __bmp_encode(FILE* out_file, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  // compute data sizes
  size_t row_bytes = in_w * in_c;
  size_t tot_bytes = row_bytes * in_h;
  size_t bmp_bytes = tot_bytes + 54; // file header + info header = 54 bytes

  // BMP headers structure
  OMM_BITMAPHEADER bmp_head = {0};
  bmp_head.signature[0] = 0x42; bmp_head.signature[1] = 0x4D; // BM signature
  bmp_head.offbits = 54; // file header + info header = 54 bytes
  bmp_head.size = bmp_bytes;

  OMM_BITMAPINFOHEADER bmp_info = {0};
  bmp_info.size = 40;
  bmp_info.width = in_w;
  bmp_info.height = in_h;
  bmp_info.planes = 1;
  bmp_info.bpp = in_c * 8;
  bmp_info.compression = 0;
  bmp_info.sizeimage = tot_bytes;
  bmp_info.xppm = bmp_info.yppm = 0x0ec4;

  // make sure we start at begining
  fseek(out_file, 0, SEEK_SET);
  // write file header
  if(fwrite(&bmp_head, 1, 14, out_file) != 14) return false;
  // write info header
  if(fwrite(&bmp_info, 1, 40, out_file) != 40) return false;

  // allocate buffer for data translation
  uint8_t* row;
  try {
    row = new uint8_t[row_bytes];
  } catch(const std::bad_alloc&) {
    return false;
  }

  // useful values for translation
  const uint8_t* sp;
  uint8_t* dp;
  unsigned hmax = (in_h - 1);

  for(unsigned y = 0; y < in_h; ++y) {
    sp = in_rgb + (row_bytes * (hmax - y)); // reverse row up to bottom
    dp = row;
    for(unsigned x = 0; x < in_w; ++x) {
      // convert RGBA to BGRA
      dp[0] = sp[2]; dp[1] = sp[1]; dp[2] = sp[0];
      if(in_c == 4) dp[3] = sp[3];
      sp += in_c; dp += in_c;
    }
    // write row to file
    if(fwrite(row, 1, row_bytes, out_file) != row_bytes) {
      delete [] row; return false;
    }
  }

  delete [] row;

  return true;
}

/// \brief Encode BMP.
///
/// Encode BMP data to buffer in memory.
///
/// \param[out] out_data  : Output BMP data, pointer to pointer to be allocated.
/// \param[out] out_size  : Output BMP data size in bytes.
/// \param[in]  in_rgb    : Input image RGB(A) data to encode.
/// \param[in]  in_w      : Input image width.
/// \param[in]  in_h      : Input image height.
/// \param[in]  in_c      : Input image color component count, either 3 or 4.
///
/// \return True if operation succeed, false otherwise
///
static bool __bmp_encode(uint8_t** out_data, size_t* out_size, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  // compute data sizes
  size_t row_bytes = in_w * in_c;
  size_t tot_bytes = row_bytes * in_h;
  size_t bmp_bytes = tot_bytes + 54;

  // BMP headers structure
  OMM_BITMAPHEADER bmp_head = {0};
  bmp_head.signature[0] = 0x42; bmp_head.signature[1] = 0x4D; // BM signature
  bmp_head.offbits = 54; // file header + info header = 54 bytes
  bmp_head.size = bmp_bytes;

  OMM_BITMAPINFOHEADER bmp_info = {0};
  bmp_info.size = 40;
  bmp_info.width = in_w;
  bmp_info.height = in_h;
  bmp_info.planes = 1;
  bmp_info.bpp = in_c * 8;
  bmp_info.compression = 0;
  bmp_info.sizeimage = tot_bytes;
  bmp_info.xppm = bmp_info.yppm = 0x0ec4;

  // allocate buffer for BMP data
  uint8_t* bmp;
  try {
    bmp = new uint8_t[bmp_bytes];
  } catch(const std::bad_alloc&) {
    return false;
  }

  // keep pointer to buffer
  uint8_t* bmp_ptr = bmp;
  // write file header
  memcpy(bmp_ptr, &bmp_head, 14); bmp_ptr += 14;
  // write info header
  memcpy(bmp_ptr, &bmp_info, 40); bmp_ptr += 40;

  // allocate buffer for data translation
  uint8_t* row;
  try {
    row = new uint8_t[row_bytes];
  } catch(const std::bad_alloc&) {
    return false;
  }

  // useful values for translation
  const uint8_t* sp;
  uint8_t* dp;
  unsigned hmax = (in_h - 1);

  for(unsigned y = 0; y < in_h; ++y) {
    sp = in_rgb + (row_bytes * (hmax - y)); // reverse row up to bottom
    dp = row;
    for(unsigned x = 0; x < in_w; ++x) {
      // convert RGBA to BGRA
      dp[0] = sp[2]; dp[1] = sp[1]; dp[2] = sp[0];
      if(in_c == 4) dp[3] = sp[3];
      sp += in_c; dp += in_c;
    }
    // write row to buffer
    memcpy(bmp_ptr, row, row_bytes); bmp_ptr += row_bytes;
  }

  delete [] row;

  (*out_data) = bmp;
  (*out_size) = bmp_bytes;

  return true;
}

/// \brief Decode JPEG.
///
/// Common function to decode JPEG using the given GIF decoder structure.
///
/// \param[in]  jpg_dec : JPEG decoder structure pointer.
/// \param[out] out_rgb : Output image RGB(A) data, pointer to pointer to be allocated.
/// \param[out] out_w   : Output image width
/// \param[out] out_h   : Output image height
/// \param[out] out_c   : Output image color component count.
/// \param[in]  flip_y  : Load image for bottom-left origin usage (upside down)
///
/// \return True if operation succeed, false otherwise
///
static bool __jpg_decode_common(void* jpg_dec, uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, bool flip_y)
{
  jpeg_decompress_struct* jpg = reinterpret_cast<jpeg_decompress_struct*>(jpg_dec);

  // read jpeg header
  if(jpeg_read_header(jpg, true) != 1)
    return false;

  // initialize decompression
  jpeg_start_decompress(jpg);

  // get image parameters
	unsigned w = jpg->output_width;
	unsigned h = jpg->output_height;
	unsigned c = jpg->output_components;

	// define sizes
  size_t row_bytes = w * c;
  size_t tot_bytes = h * row_bytes;

  // allocate buffer to receive RGB data
  uint8_t* rgb;
  try {
    rgb = new uint8_t[tot_bytes];
  } catch(const std::bad_alloc&) {
    return false;
  }

  // row list pointer for jpeg decoder
  uint8_t* rows[1];

  if(flip_y) {
    unsigned hmax = h - 1;
    while (jpg->output_scanline < jpg->output_height) {
      rows[0] = rgb + ((hmax - jpg->output_scanline) * row_bytes);
      jpeg_read_scanlines(jpg, rows, 1); //< read one row (scanline)
    }
  } else {
    while (jpg->output_scanline < jpg->output_height) {
      rows[0] = rgb + (jpg->output_scanline * row_bytes);
      jpeg_read_scanlines(jpg, rows, 1); //< read one row (scanline)
    }
  }

	// cleanup
	jpeg_finish_decompress(jpg);
	jpeg_destroy_decompress(jpg);

	(*out_rgb) = rgb; (*out_w) = w; (*out_h) = h; (*out_c) = c;

	return true;
}

/// \brief Encode JEPG.
///
/// Common function to encode JEPG using the given JEPG encoder structure.
///
/// \param[in]  jpg_enc : JEPG encoder structure pointer.
/// \param[in]  in_rgb  : Input image RGB(A) data to encode.
/// \param[in]  in_w    : Input image width.
/// \param[in]  in_h    : Input image height.
/// \param[in]  in_c    : Input image color component count.
/// \param[in]  level   : JPEG compression quality level 0 to 100.
///
/// \return True if operation succeed, false otherwise
///
static bool __jpg_encode_common(void* jpg_enc, uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c, int level)
{
  jpeg_compress_struct* jpg = reinterpret_cast<jpeg_compress_struct*>(jpg_enc);

  // define image parameters
  jpg->image_width = in_w;        //< Image width in pixels
  jpg->image_height = in_h;       //< Image height in pixels
  jpg->input_components = 3;      //< per pixel color components
  jpg->in_color_space = JCS_RGB;  //< pixel format

  // set parameters to encoder
  jpeg_set_defaults(jpg);
  // set compression quality
  jpeg_set_quality(jpg, level, true); // quality is 0-100 scaled
  // initialize encoder
  jpeg_start_compress(jpg, true);

  // hold row size in bytes
  unsigned row_bytes = in_w * in_c;

  uint8_t* rows[1];

  if(in_c == 4) {
    // JPEG encoder does not handle RGBA source we must convert data
    uint8_t* sp;
    uint8_t* dp;
    // create new buffer for one RGB row
    try {
      rows[0] = new uint8_t[in_w * 3];
    } catch(const std::bad_alloc&) {
      return false;
    }
    // give RGB data to JPEG encoder
    while(jpg->next_scanline < jpg->image_height) {
      // set source and destination pointers
      sp = in_rgb + (jpg->next_scanline * row_bytes);
      dp = rows[0];
      // convert RGBA to RGB
      for(unsigned i = 0; i < in_w; ++i) {
        dp[0] = sp[0]; dp[1] = sp[4]; dp[2] = sp[3];
        sp += 4; dp += 3;
      }
      // send to encoder
      jpeg_write_scanlines(jpg, rows, 1);
    }
    delete [] rows[0];
  } else {
    // give RGB data to JPEG encoder
    while(jpg->next_scanline < jpg->image_height) {
      // get pointer to rows
      rows[0] = in_rgb + (jpg->next_scanline * row_bytes);
      // send to encoder
      jpeg_write_scanlines(jpg, rows, 1);
    }
  }

  // finalize compression
  jpeg_finish_compress(jpg);
  // destroy encoder
  jpeg_destroy_compress(jpg);

  return true;
}

/// \brief Decode JPEG.
///
/// Decode JPEG data from file pointer.
///
/// \param[out] out_rgb : Output image RGB(A) data, pointer to pointer to be allocated.
/// \param[out] out_w   : Output image width
/// \param[out] out_h   : Output image height
/// \param[out] out_c   : Output image color component count.
/// \param[in]  in_file : Input file pointer to read data from.
/// \param[in]  flip_y  : Load image for bottom-left origin usage (upside down)
///
/// \return True if operation succeed, false otherwise
///
static int __jpg_decode(uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, FILE* in_file, bool flip_y)
{
  // create base object for jpeg decoder
  jpeg_decompress_struct jpg;
  jpeg_error_mgr jerr;

  // create jpeg decoder
  jpg.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&jpg);

  // make sure we start at beginning and setup jpeg IO
  fseek(in_file, 0, SEEK_SET);
  jpeg_stdio_src(&jpg, in_file);

  return __jpg_decode_common(&jpg, out_rgb, out_w, out_h, out_c, flip_y);
}

/// \brief Decode JPEG.
///
/// Decode JPEG data from buffer in memory.
///
/// \param[out] out_rgb : Output image RGB(A) data, pointer to pointer to be allocated.
/// \param[out] out_w   : Output image width
/// \param[out] out_h   : Output image height
/// \param[out] out_c   : Output image color component count.
/// \param[in]  in_data : Input JPEG data to decode.
/// \param[in]  in_size : Input JPEG data size in bytes.
/// \param[in]  flip_y  : Load image for bottom-left origin usage (upside down)
///
/// \return True if operation succeed, false otherwise
///
static int __jpg_decode(uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, uint8_t* in_data, size_t in_size, bool flip_y)
{
  // create base object for jpeg decoder
  jpeg_decompress_struct jpg;
  jpeg_error_mgr jerr;

  // create jpeg decoder
  jpg.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&jpg);

  // set read data pointer
  jpeg_mem_src(&jpg, in_data, in_size);

  return __jpg_decode_common(&jpg, out_rgb, out_w, out_h, out_c, flip_y);
}

/// \brief Encode JPEG.
///
/// Encode JPEG data to file pointer.
///
/// \param[out] out_file  : File pointer to write to.
/// \param[in]  in_rgb    : Input image RGB(A) data to encode.
/// \param[in]  in_w      : Input image width.
/// \param[in]  in_h      : Input image height.
/// \param[in]  in_c      : Input image color component count, either 3 or 4.
/// \param[in]  level     : JPEG compression quality level 0 to 10.
///
/// \return True if operation succeed, false otherwise
///
static bool __jpg_encode(FILE* out_file, uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c, int level)
{

  // create base object for jpeg encoder
  jpeg_compress_struct jpg;
  jpeg_error_mgr jerr;

  // create jpeg encoder
  jpg.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&jpg);

  // make sure we start at beginning and setup jpeg IO
  fseek(out_file, 0, SEEK_SET);
  jpeg_stdio_dest(&jpg, out_file);

  return __jpg_encode_common(&jpg, in_rgb, in_w, in_h, in_c, level * 10);
}

/// \brief Encode JPEG.
///
/// Encode JPEG data to buffer in memory.
///
/// \param[out] out_data  : Output JPEG data, pointer to pointer to be allocated.
/// \param[out] out_size  : Output JPEG data size in bytes.
/// \param[in]  in_rgb    : Input image RGB(A) data to encode.
/// \param[in]  in_w      : Input image width.
/// \param[in]  in_h      : Input image height.
/// \param[in]  in_c      : Input image color component count, either 3 or 4.
/// \param[in]  level     : JPEG compression quality level 0 to 10.
///
/// \return True if operation succeed, false otherwise
///
static bool __jpg_encode(uint8_t** out_data, size_t* out_size, uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c, int level)
{

  // create base object for jpeg encoder
  jpeg_compress_struct jpg;
  jpeg_error_mgr jerr;

  // create jpeg encoder
  jpg.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&jpg);

  // set pointer params
  unsigned long jpg_size;
  jpeg_mem_dest(&jpg, out_data, &jpg_size);

  if(!__jpg_encode_common(&jpg, in_rgb, in_w, in_h, in_c, level * 10))
    return false;

  (*out_size) = jpg_size;

  return true;
}

/// \brief Decode PNG.
///
/// Common function to decode PNG using the given PNG decoder structure.
///
/// \param[in]  png_dec : PNG decoder structure pointer.
/// \param[out] out_rgb : Output image RGB(A) data, pointer to pointer to be allocated.
/// \param[out] out_w   : Output image width
/// \param[out] out_h   : Output image height
/// \param[out] out_c   : Output image color component count.
/// \param[in]  flip_y  : Load image for bottom-left origin usage (upside down)
///
/// \return True if operation succeed, false otherwise
///
static bool __png_decode_common(void* png_dec, uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, bool flip_y)
{
  // get decoder
  png_structp png = reinterpret_cast<png_structp>(png_dec);

  // create PNG info structure
  png_infop png_info = png_create_info_struct(png);

  // get image properties
  png_read_info(png, png_info);
  unsigned w = png_get_image_width(png, png_info);
  unsigned h = png_get_image_height(png, png_info);
  unsigned c = png_get_channels(png, png_info);

  // retrieve and define useful sizes
  size_t row_bytes = png_get_rowbytes(png, png_info);
  size_t tot_bytes = h * row_bytes;

  // allocate pointer to receive RGB(A) data
  uint8_t* rgb;
  try {
    rgb = new uint8_t[tot_bytes];
  } catch(const std::bad_alloc&) {
    return false;
  }

  // we need an array of pointers, with one pointer per row
  uint8_t** rows;
  try {
    rows = new uint8_t*[h];
  } catch(const std::bad_alloc&) {
    delete [] rgb;
    return false;
  }

  // setup each pointer to a destination row in destination buffer
  if(flip_y) {
    unsigned hmax = h - 1;
    for(unsigned y = 0; y < h; y++)
      rows[y] = rgb + ((hmax - y) * row_bytes);
  } else {
    for(unsigned y = 0; y < h; y++)
      rows[y] = rgb + (y * row_bytes);
  }

  // read all rows at once
  png_read_image(png, rows);

  // cleanup
  png_destroy_read_struct(&png, &png_info, nullptr);

  delete[] rows;

  // assign output values
  (*out_rgb) = rgb; (*out_w) = w; (*out_h) = h; (*out_c) = c;

  return true;
}

/// \brief Encode PNG.
///
/// Common function to encode PNG using the given PNG encoder structure.
///
/// \param[in]  png_enc : PNG encoder structure pointer.
/// \param[in]  in_rgb  : Input image RGB(A) data to encode.
/// \param[in]  in_w    : Input image width.
/// \param[in]  in_h    : Input image height.
/// \param[in]  in_c    : Input image color component count.
/// \param[in]  level   : PNG compression level 0 to 9.
///
/// \return True if operation succeed, false otherwise
///
static bool __png_encode_common(void* png_enc, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c, int level)
{
  // get encoder
  png_structp png = reinterpret_cast<png_structp>(png_enc);

  // create PNG info structure
  png_infop png_info = png_create_info_struct(png);

  // set PNG parameters
  png_set_IHDR(png, png_info, in_w, in_h, 8,
               (in_c == 4) ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  // set compression level
  if(level > 9) level = 9; // clamp to 9
  png_set_compression_level(png, level);

  // write info to PNG
  png_write_info(png, png_info);

  // define useful sizes
  size_t row_bytes = in_w * in_c;

  // send RGB data to PNG encoder
  const uint8_t* row;
  for(unsigned y = 0; y < in_h; ++y) {
    row = in_rgb + (y * row_bytes);
    png_write_row(png, row);
  }

  // clear PGN encoder
  png_free_data(png, png_info, PNG_FREE_ALL, -1);
  png_destroy_write_struct(&png, &png_info);

  return true;
}

/// \brief Decode PNG.
///
/// Decode PNG data from file pointer.
///
/// \param[out] out_rgb : Output image RGB(A) data, pointer to pointer to be allocated.
/// \param[out] out_w   : Output image width
/// \param[out] out_h   : Output image height
/// \param[out] out_c   : Output image color component count.
/// \param[in]  in_file : Input file pointer to read data from.
/// \param[in]  flip_y  : Load image for bottom-left origin usage (upside down)
///
/// \return True if operation succeed, false otherwise
///
static int __png_decode(uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, FILE* in_file, bool flip_y)
{
  // create PNG decoder structure
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

  // make sure we start at beginning and setup png IO
  fseek(in_file, 0, SEEK_SET);
  png_init_io(png, in_file);

  // decode PNG data
  return __png_decode_common(png, out_rgb, out_w, out_h, out_c, flip_y);
}

/// \brief Custom PNG read struct
///
/// Custom structure for custom PNG read function
///
struct __png_read_st {
  const uint8_t*  src_data;
  size_t          src_seek;
};

/// \brief Custom PNG read
///
/// Custom read function for PNG library to read from memory.
///
/// \param[in]  png     Decoder structure pointer.
/// \param[in]  dst     Destination buffer.
/// \param[in]  len     Length of data that should be read.
///
void __png_read_buff_fn(png_structp png, uint8_t* dst, size_t len)
{
  __png_read_st *read_st = static_cast<__png_read_st*>(png_get_io_ptr(png));
  memcpy(dst, read_st->src_data + read_st->src_seek, len);
  read_st->src_seek += len;
}

/// \brief Decode PNG.
///
/// Decode PNG data from buffer in memory.
///
/// \param[out] out_rgb : Output image RGB(A) data, pointer to pointer to be allocated.
/// \param[out] out_w   : Output image width
/// \param[out] out_h   : Output image height
/// \param[out] out_c   : Output image color component count.
/// \param[in]  in_data : Input GIF data to decode.
/// \param[in]  flip_y  : Load image for bottom-left origin usage (upside down)
///
/// \return True if operation succeed, false otherwise
///
static int __png_decode(uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, const uint8_t* in_data, bool flip_y)
{
  // create PNG decoder structure
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

  // custom read structure
  __png_read_st read_st;
  read_st.src_data = in_data;
  read_st.src_seek = 0;

  // set custom read process
  png_set_read_fn(png, &read_st, __png_read_buff_fn);

  // decode PNG data
  return __png_decode_common(png, out_rgb, out_w, out_h, out_c, flip_y);
}

/// \brief Encode PNG.
///
/// Encode PNG data to file pointer.
///
/// \param[out] out_file  : File pointer to write to.
/// \param[in]  in_rgb    : Input image RGB(A) data to encode.
/// \param[in]  in_w      : Input image width.
/// \param[in]  in_h      : Input image height.
/// \param[in]  in_c      : Input image color component count, either 3 or 4.
/// \param[in]  level     : PNG compression level 0 to 9.
///
/// \return True if operation succeed, false otherwise
///
static bool __png_encode(FILE* out_file, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c, int level)
{
  // create PNG encoder structure
  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

  // make sure we start at beginning and setup png IO
  fseek(out_file, 0, SEEK_SET);
  png_init_io(png, out_file);

  return __png_encode_common(png, in_rgb, in_w, in_h, in_c, level);
}

/// \brief Custom PNG write struct
///
/// Custom structure for custom PNG write function
///
struct __png_write_st {
  uint8_t*  dst_data;
  size_t    dst_size;
  size_t    dst_seek;
};

/// \brief Custom PNG writer
///
/// Custom write function for PNG library to encode to memory.
///
/// \param[in]  png     Decoder structure pointer.
/// \param[in]  src     Source data buffer.
/// \param[in]  len     Length of data that should be written.
///
void __png_write_buff_fn(png_structp png, uint8_t* src, size_t len)
{
  __png_write_st *write_st = static_cast<__png_write_st*>(png_get_io_ptr(png));
  write_st->dst_size += len;
  if(write_st->dst_data) {
    write_st->dst_data = reinterpret_cast<uint8_t*>(realloc(write_st->dst_data, write_st->dst_size));
  } else {
    write_st->dst_data = reinterpret_cast<uint8_t*>(malloc(write_st->dst_size));
  }
  if(!write_st->dst_data) png_error(png, "alloc error in __png_write_fn");
  memcpy(write_st->dst_data + write_st->dst_seek, src, len);
  write_st->dst_seek += len;
}

/// \brief Custom PNG flush function
///
/// Custom callback function for PNG encoder flush.
///
void __png_flush_fn(png_structp png)
{
}

/// \brief Encode PNG.
///
/// Common function to encode PNG using the given PNG encoder structure.
///
/// \param[in]  gif_enc : PNG encoder structure pointer.
/// \param[in]  in_rgb  : Input image RGB(A) data to encode.
/// \param[in]  in_w    : Input image width.
/// \param[in]  in_h    : Input image height.
/// \param[in]  in_c    : Input image color component count.
/// \param[in]  level     : PNG compression level 0 to 9.
///
/// \return True if operation succeed, false otherwise
///
static bool __png_encode(uint8_t** out_data, size_t* out_size, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c, int level)
{
  // create PNG encoder structure
  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

  // custom write structure
  __png_write_st write_st;
  write_st.dst_data = nullptr;
  write_st.dst_size = 0;
  write_st.dst_seek = 0;

  // custom write process
  png_set_write_fn(png, &write_st, __png_write_buff_fn, __png_flush_fn);

  if(!__png_encode_common(png, in_rgb, in_w, in_h, in_c, level))
    return false;

  (*out_data) = write_st.dst_data;
  (*out_size) = write_st.dst_size;

  return true;
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
int Om_loadImage(uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, FILE* in_file, bool flip_y)
{
  // read first 8 bytes of the file
  unsigned char buff[8];
  fseek(in_file, 0, SEEK_SET);
  if(fread(buff, 1, 8, in_file) < 8)
    return false;

  int type = __image_sign_matches(buff);
  if(type != 0) {
    // check for known image file signatures
    switch(type)
    {
    case OMM_IMAGE_TYPE_BMP:
      if(!__bmp_decode(out_rgb, out_w, out_h, out_c, in_file, flip_y)) return -1;
      break;
    case OMM_IMAGE_TYPE_JPG:
      if(!__jpg_decode(out_rgb, out_w, out_h, out_c, in_file, flip_y)) return -1;
      break;
    case OMM_IMAGE_TYPE_PNG:
      if(!__png_decode(out_rgb, out_w, out_h, out_c, in_file, flip_y)) return -1;
      break;
    case OMM_IMAGE_TYPE_GIF:
      if(!__gif_decode(out_rgb, out_w, out_h, out_c, in_file, flip_y)) return -1;
      break;
    }
  }

  return type;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
int Om_loadImage(uint8_t** out_rgb, unsigned* out_w, unsigned* out_h, unsigned* out_c, uint8_t* in_data, size_t in_size, bool flip_y)
{
  int type = __image_sign_matches(in_data);
  if(type != 0) {
    // check for known image file signatures
    switch(type)
    {
    case OMM_IMAGE_TYPE_BMP:
      if(!__bmp_decode(out_rgb, out_w, out_h, out_c, in_data, flip_y)) return -1;
      break;
    case OMM_IMAGE_TYPE_JPG:
      if(!__jpg_decode(out_rgb, out_w, out_h, out_c, in_data, in_size, flip_y)) return -1;
      break;
    case OMM_IMAGE_TYPE_PNG:
      if(!__png_decode(out_rgb, out_w, out_h, out_c, in_data, flip_y)) return -1;
      break;
    case OMM_IMAGE_TYPE_GIF:
      if(!__gif_decode(out_rgb, out_w, out_h, out_c, in_data, flip_y)) return -1;
      break;
    }
  }

  return type;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_saveBmp(const wstring& path, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  // open file
  FILE* fp;

  //if((fp = fopen(Om_toUtf8(path).c_str(), "rb")) == nullptr) {
  if((fp = _wfopen(path.c_str(), L"wb")) == nullptr)
    return false;

  bool result = __bmp_encode(fp, in_rgb, in_w, in_h, in_c);

  fclose(fp);

  return result;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_saveJpg(const wstring& path, uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c, int level)
{
  // open file
  FILE* fp;

  //if((fp = fopen(Om_toUtf8(path).c_str(), "rb")) == nullptr) {
  if((fp = _wfopen(path.c_str(), L"wb")) == nullptr)
    return false;

  bool result = __jpg_encode(fp, in_rgb, in_w, in_h, in_c, level);

  fclose(fp);

  return result;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_savePng(const wstring& path, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c, int level)
{
  // open file
  FILE* fp;

  //if((fp = fopen(Om_toUtf8(path).c_str(), "rb")) == nullptr) {
  if((fp = _wfopen(path.c_str(), L"wb")) == nullptr)
    return false;

  bool result = __png_encode(fp, in_rgb, in_w, in_h, in_c, level);

  fclose(fp);

  return result;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_saveGif(const wstring& path, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  // open file
  FILE* fp;

  //if((fp = fopen(Om_toUtf8(path).c_str(), "rb")) == nullptr) {
  if((fp = _wfopen(path.c_str(), L"wb")) == nullptr)
    return false;

  bool result = __gif_encode(fp, in_rgb, in_w, in_h, in_c);

  fclose(fp);

  return result;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_encodeBmp(uint8_t** out_data, size_t* out_size, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  return __bmp_encode(out_data, out_size, in_rgb, in_w, in_h, in_c);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_encodeJpg(uint8_t** out_data, size_t* out_size, uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c, int level)
{
  return __jpg_encode(out_data, out_size, in_rgb, in_w, in_h, in_c, level);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_encodePng(uint8_t** out_data, size_t* out_size, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c, int level)
{
  return __png_encode(out_data, out_size, in_rgb, in_w, in_h, in_c, level);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool Om_encodeGif(uint8_t** out_data, size_t* out_size, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  return __gif_encode(out_data, out_size, in_rgb, in_w, in_h, in_c);
}


/// \brief Compute cubic interpolation.
///
/// Compute cubic the interpolation of the specified values.
///
/// \param[in]  a : Start handle
/// \param[in]  b : Start node
/// \param[in]  c : End node
/// \param[in]  d : End handle
/// \param[in]  t : Interpolation phase
///
/// \return cubic interpolated value
///
static inline float __interp_cubic(float a, float b, float c, float d, float t)
{
  return b + 0.5f * t * (c - a + t * (2.0f * a - 5.0f * b + 4.0f * c - d + t * (3.0f * (b - c) + d - a)));
}

/// \brief Get BiCubic interpolated pixel.
///
/// Compute the BiCubic interpolated pixel at the specified coordinates.
///
/// \param[in]  out_pix : Array to receive interpolated pixel components.
/// \param[in]  u       : Sample horizontal coordinate in image, from 0.0 to 1.0.
/// \param[in]  v       : Sample vertical coordinate in image, from 0.0 to 1.0.
/// \param[in]  in_rgb  : Input image RGB(A) data.
/// \param[in]  in_w    : Input image width.
/// \param[in]  in_h    : Input image height.
/// \param[in]  in_c    : Input image color component count (bytes per pixel).
///
static inline void __image_sample_bicubic(uint8_t* out_pix, float u, float v, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  float x, y;

  float xf = modf((u * in_w) - 0.5f, &x);
  float yf = modf((v * in_h) - 0.5f, &y);

  int ix = static_cast<int>(x) - 1;
  int iy = static_cast<int>(y) - 1;

  const uint8_t* sp[4];

  float r[4]; float g[4]; float b[4]; float a[4];

  int xx, yy, ys;
  int hmax = in_h - 1;
  int wmax = in_w - 1;

  for(int j = 0; j < 4; ++j) {
    yy = std::max(0, std::min(iy + j, hmax));
    ys = (yy * (in_w * in_c));
    for(int i = 0; i < 4; ++i) {
      xx = std::max(0, std::min(ix + i, wmax));
      sp[i] = in_rgb + (ys + (xx * in_c));
    }
    r[j] = __interp_cubic(sp[0][0], sp[1][0], sp[2][0], sp[3][0], xf);
    g[j] = __interp_cubic(sp[0][1], sp[1][1], sp[2][1], sp[3][1], xf);
    b[j] = __interp_cubic(sp[0][2], sp[1][2], sp[2][2], sp[3][2], xf);
    if(in_c == 4) a[j] = __interp_cubic(sp[0][3], sp[1][3], sp[2][3], sp[3][3], xf);
  }

  float m[4];
  m[0] = __interp_cubic(r[0], r[1], r[2], r[3], yf);
  m[1] = __interp_cubic(g[0], g[1], g[2], g[3], yf);
  m[2] = __interp_cubic(b[0], b[1], b[2], b[3], yf);
  if(in_c == 4) m[3] = __interp_cubic(a[0], a[1], a[2], a[3], yf);

  for(unsigned i = 0; i < in_c; ++i) {
    out_pix[i] = static_cast<uint8_t>(std::max(0.0f, std::min(m[i], 255.0f)));
  }
}


/// \brief Get box interpolated pixel.
///
/// Compute the box interpolated pixel at the specified coordinates.
///
/// \param[in]  out_pix : Array to receive interpolated pixel components.
/// \param[in]  box_w   : Box width in pixels.
/// \param[in]  box_h   : Box height in pixels.
/// \param[in]  u       : Sample horizontal coordinate in image, from 0.0 to 1.0.
/// \param[in]  v       : Sample vertical coordinate in image, from 0.0 to 1.0.
/// \param[in]  in_rgb  : Input image RGB(A) data.
/// \param[in]  in_w    : Input image width.
/// \param[in]  in_h    : Input image height.
/// \param[in]  in_c    : Input image color component count (bytes per pixel).
///
static inline void __image_sample_box(uint8_t* out_pix, int box_w, int box_h, float u, float v, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  float r = 0.0f; float g = 0.0f; float b = 0.0f; float a = 0.0f;

  const uint8_t *sp;

  int xx, yy;
  int hmax = (in_h - 1);
  int wmax = (in_w - 1);

  int x = u * hmax;
  int y = v * wmax;

  for(int j = 0; j < box_h; ++j) {
    yy = std::max(0, std::min(y + j, hmax));
    for(int i = 0; i < box_w; ++i) {
      xx = std::max(0, std::min(x + i, wmax));
      sp = in_rgb + ((xx + yy * in_w) * in_c);
      r += sp[0]; g += sp[1]; b += sp[2];
      if(in_c == 4) a += sp[3];
    }
  }

  float f = box_h * box_w;

  out_pix[0] = r / f; out_pix[1] = g / f; out_pix[2] = b / f;
  if(in_c == 4) out_pix[3] = a / f;
}


/// \brief Box filter downsample image.
///
/// Reduce image resolution using box filtering.
///
/// \param[out] out_rgb : Output destination pointer.
/// \param[in]  w       : target width.
/// \param[in]  w       : target height.
/// \param[in]  in_rgb  : Source image RGB(A) data.
/// \param[in]  in_w    : Source image width.
/// \param[in]  in_h    : source image height.
/// \param[in]  in_c    : Source image component count (bytes per pixel)
///
static void __image_dsample(uint8_t* out_rgb, unsigned w, unsigned h, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  unsigned box_w = floor(static_cast<float>(in_w) / w);
  unsigned box_h = floor(static_cast<float>(in_h) / h);

  float inv_w = 1.0f / (static_cast<float>(w) - 1);
  float inv_h = 1.0f / (static_cast<float>(h) - 1);

  uint8_t smpl[4];

  uint8_t* dp;
  float u, v;

  for(unsigned y = 0; y < h; ++y) {
    dp = out_rgb + (w * in_c * y);
    v = y * inv_h;
    for(unsigned x = 0; x < w; ++x) {
      u = x * inv_w;
      __image_sample_box(smpl, box_w, box_h, u, v, in_rgb, in_w, in_h, in_c);
      dp[0] = smpl[0]; dp[1] = smpl[1]; dp[2] = smpl[2];
      if(in_c == 4) dp[3] = smpl[3];
      dp += in_c;
    }
  }
}

/// \brief Bicubic filter upsample image
///
/// Increase image resolution using bicubic filtering.
///
/// \param[out] out_rgb : Output destination pointer.
/// \param[in]  w       : target width.
/// \param[in]  w       : target height.
/// \param[in]  in_rgb  : Source image RGB(A) data.
/// \param[in]  in_w    : Source image width.
/// \param[in]  in_h    : source image height.
/// \param[in]  in_c    : Source image component count (bytes per pixel)
///
static void __image_usample(uint8_t* out_rgb, unsigned w, unsigned h, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  float inv_w = 1.0f / (static_cast<float>(w) - 1);
  float inv_h = 1.0f / (static_cast<float>(h) - 1);

  uint8_t smpl[4];

  uint8_t* dp;
  float u, v;

  for(unsigned y = 0; y < h; ++y) {
    dp = out_rgb + ((w * in_c) * y);
    v = y * inv_h;
    for(unsigned x = 0; x < w; ++x) {
      u = x * inv_w;
      __image_sample_bicubic(smpl, u, v, in_rgb, in_w, in_h, in_c);
      dp[0] = smpl[0]; dp[1] = smpl[1]; dp[2] = smpl[2];
      if(in_c == 4) dp[3] = smpl[3];
      dp += in_c;
    }
  }
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
uint8_t* Om_resizeImage(unsigned w, unsigned h, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  uint8_t* out_rgb;
  try {
    out_rgb = new uint8_t[w * in_c * h];
  } catch(const std::bad_alloc&) {
    return nullptr;
  }

  // resize image to fit desired square
  if(in_w != w || in_h != h) {

    if(w > in_w || h > in_w) {
      __image_usample(out_rgb, w, h, in_rgb, in_w, in_h, in_c);
    } else {
      __image_dsample(out_rgb, w, h, in_rgb, in_w, in_h, in_c);
    }

  } else {

    memcpy(out_rgb, in_rgb, (in_w * in_c) * in_h);
  }

  return out_rgb;
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
uint8_t* Om_cropImage(unsigned x, unsigned y, unsigned w, unsigned h, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  if((x + w) > in_w || (y + h) > in_h)
    return nullptr;

  // define useful sizes
  size_t in_row_bytes = in_w * in_c;
  size_t out_row_bytes = w * in_c;

  // allocate new buffer for cropped data
  uint8_t* out_rgb;
  try {
    out_rgb = new uint8_t[out_row_bytes * h];
  } catch(const std::bad_alloc&) {
    return nullptr;
  }

  // copy required RGB data
  const uint8_t* sp;
  uint8_t* dp;

  unsigned x_shift = (x * in_c);

  for(unsigned j = 0; j < h; ++j) {

    dp = out_rgb + (out_row_bytes * j);
    sp = in_rgb + ((in_row_bytes * (j + y)) + x_shift);

    for(unsigned i = 0; i < w; ++i) {

      dp[0] = sp[0]; dp[1] = sp[1]; dp[2] = sp[2];
      if(in_c == 4) dp[3] = sp[3];

      dp += in_c; sp += in_c;
    }
  }

  return out_rgb;
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
uint8_t* Om_thumbnailImage(unsigned size, const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  // create locale copy of original data
  uint8_t* out_rgb;
  try {
    out_rgb = new uint8_t[(in_w * in_c) * in_h];
  } catch(const std::bad_alloc&) {
    return nullptr;
  }
  memcpy(out_rgb, in_rgb, (in_w * in_c) * in_h);

  // resize image to fit desired size
  if(in_w != size || in_h != size) {

    unsigned out_w, out_h;
    // determine target size according image aspect ratio
    float a = static_cast<float>(in_w) / in_h;
    if(a > 1.0f) {
      out_w = static_cast<float>(size) * a;
      out_h = size;
    } else {
      out_w = size;
      out_h = static_cast<float>(size) / a;
    }

    uint8_t* tmp_rgb = Om_resizeImage(out_w, out_h, out_rgb, in_w, in_h, in_c);
    if(tmp_rgb == nullptr) {
      return nullptr;
    }

    // swap buffers
    delete [] out_rgb;
    out_rgb = tmp_rgb;

    // update input width and height
    in_h = out_h;
    in_w = out_w;
  }

  // crop image to square
  if(in_w != in_h) {

    unsigned x, y;

    if(in_w > in_h) {
      x = (in_w * 0.5f) - (in_h * 0.5f);
      y = 0;
    } else {
      x = 0;
      y = (in_h * 0.5f) - (in_w * 0.5f);
    }

    uint8_t* tmp_rgb = Om_cropImage(x, y, size, size, out_rgb, in_w, in_h, in_c);
    if(tmp_rgb == nullptr) {
      return nullptr;
    }

    // swap buffers
    delete [] out_rgb;
    out_rgb = tmp_rgb;
  }

  return out_rgb;
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
HBITMAP Om_hbitmapImage(const uint8_t* in_rgb, unsigned in_w, unsigned in_h, unsigned in_c)
{
  // destination buffer is in RGBA
  size_t row_bytes = (in_w * 4);
  size_t tot_bytes = row_bytes * in_h;

  uint8_t* rgba;
  try {
    rgba = new uint8_t[tot_bytes];
  } catch(const std::bad_alloc&) {
    return nullptr;
  }

  if(in_c == 3) {

    size_t in_row_bytes = (in_w * 3);

    // copy data and convert RGB to BGRA
    const uint8_t *sp;
    uint8_t *dp;

    for(unsigned y = 0; y < in_h; ++y) {
      dp = rgba + (row_bytes * y);
      sp = in_rgb + (in_row_bytes * y);
      for(unsigned x = 0; x < in_w; ++x) {
        // swapt RGB to BGR
        dp[0] = sp[2]; dp[1] = sp[1]; dp[2] = sp[0];
        dp[3] = 0xff;
        sp += 3; dp += 4;
      }
    }

  } else {

    // simply copy original data
    memcpy(rgba, in_rgb, tot_bytes);

    // swap components order RGB to BGR
    for(unsigned i = 0; i < tot_bytes; i += 4)
      rgba[i  ] ^= rgba[i+2] ^= rgba[i  ] ^= rgba[i+2]; //< RGB => BGR
  }

  return CreateBitmap(in_w, in_h, 1, 32, rgba);
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
HICON Om_loadShellIcon(unsigned id, bool large)
{
  SHSTOCKICONINFO sIi = {};
  sIi.cbSize = sizeof(SHSTOCKICONINFO);

  SHGetStockIconInfo(static_cast<SHSTOCKICONID>(id),
                    (large) ? SHGSI_ICON|SHGSI_LARGEICON : SHGSI_ICON|SHGSI_SMALLICON,
                    &sIi);
  return sIi.hIcon;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
HBITMAP Om_loadShellBitmap(unsigned id, bool large)
{
  SHSTOCKICONINFO sIi = {};
  sIi.cbSize = sizeof(SHSTOCKICONINFO);

  SHGetStockIconInfo(static_cast<SHSTOCKICONID>(id),
                    (large) ? SHGSI_ICON|SHGSI_LARGEICON : SHGSI_ICON|SHGSI_SMALLICON,
                    &sIi);

  ICONINFO iCi = {};
  GetIconInfo(sIi.hIcon, &iCi);
  HBITMAP result = static_cast<HBITMAP>(CopyImage(iCi.hbmColor, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_COPYDELETEORG));
  DestroyIcon(sIi.hIcon);

  return result;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
HFONT Om_createFont(unsigned pt, unsigned weight, const wchar_t* name)
{
  return CreateFontW( pt, 0, 0, 0, weight,
                      false, false, false,
                      ANSI_CHARSET,
                      OUT_TT_PRECIS, 0, CLEARTYPE_QUALITY, 0,
                      name);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
wstring Om_getErrorStr(int code) {

  wchar_t num_buf[32];
  swprintf(num_buf, 32, L"%x", code);

  wstring ret = L"(0x"; ret.append(num_buf); ret.append(L") ");

  switch(code)
  {
  case ERROR_FILE_NOT_FOUND: ret.append(L"FILE_NOT_FOUND"); break;
  case ERROR_PATH_NOT_FOUND: ret.append(L"PATH_NOT_FOUND"); break;
  case ERROR_TOO_MANY_OPEN_FILES: ret.append(L"TOO_MANY_OPEN_FILES"); break;
  case ERROR_ACCESS_DENIED: ret.append(L"ACCESS_DENIED"); break;
  case ERROR_ARENA_TRASHED: ret.append(L"ARENA_TRASHED"); break;
  case ERROR_NOT_ENOUGH_MEMORY: ret.append(L"NOT_ENOUGH_MEMORY"); break;
  case ERROR_INVALID_BLOCK: ret.append(L"INVALID_BLOCK"); break;
  case ERROR_INVALID_ACCESS: ret.append(L"INVALID_ACCESS"); break;
  case ERROR_INVALID_DATA: ret.append(L"INVALID_DATA"); break;
  case ERROR_OUTOFMEMORY: ret.append(L"OUTOFMEMORY"); break;
  case ERROR_INVALID_DRIVE: ret.append(L"INVALID_DRIVE"); break;
  case ERROR_CURRENT_DIRECTORY: ret.append(L"CURRENT_DIRECTORY"); break;
  case ERROR_NOT_SAME_DEVICE: ret.append(L"NOT_SAME_DEVICE"); break;
  case ERROR_WRITE_PROTECT: ret.append(L"WRITE_PROTECT"); break;
  case ERROR_CRC: ret.append(L"ERROR_CRC"); break;
  case ERROR_SEEK: ret.append(L"ERROR_SEEK"); break;
  case ERROR_WRITE_FAULT: ret.append(L"WRITE_FAULT"); break;
  case ERROR_READ_FAULT: ret.append(L"READ_FAULT"); break;
  case ERROR_SHARING_VIOLATION: ret.append(L"SHARING_VIOLATION"); break;
  case ERROR_LOCK_VIOLATION: ret.append(L"LOCK_VIOLATION"); break;
  case ERROR_WRONG_DISK: ret.append(L"WRONG_DISK"); break;
  case ERROR_HANDLE_DISK_FULL: ret.append(L"HANDLE_DISK_FULL"); break;
  case ERROR_FILE_EXISTS: ret.append(L"FILE_EXISTS"); break;
  case ERROR_DRIVE_LOCKED: ret.append(L"DRIVE_LOCKED"); break;
  case ERROR_OPEN_FAILED: ret.append(L"OPEN_FAILED"); break;
  case ERROR_BUFFER_OVERFLOW: ret.append(L"BUFFER_OVERFLOW"); break;
  case ERROR_DISK_FULL: ret.append(L"DISK_FULL"); break;
  case ERROR_INVALID_NAME: ret.append(L"INVALID_NAME"); break;
  case ERROR_DIR_NOT_EMPTY: ret.append(L"DIR_NOT_EMPTY"); break;
  case ERROR_ALREADY_EXISTS: ret.append(L"ALREADY_EXISTS"); break;
  }

  return ret;
}
