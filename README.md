# hangultrans_bjw
This repository contains a Python package intended for distribution on PyPI. It provides a convenient utility designed to address longstanding issues related to Korean text encoding in UTF-8, helping developers handle and process Hangul characters more reliably and efficiently.

Each character is converted into:

- 1 byte: 초성 (initial consonant)
- 1 byte: 중성 (vowel)
- 1 byte: 종성 (final consonant)

Missing components are represented as `0x00`.

---

## Installation

'''bash
pip install hangultrans
'''

## Examples

```python
import hangultrans

print(hangultrans.h("안녕하세요"))
print(hangultrans.h_hex("안녕하세요"))
```

### result

```
['0C 01 04', '02 05 00', '0B 05 00', '0C 05 00', '0B 0D 00']
```