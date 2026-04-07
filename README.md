# hangultrans_bjw
This repository contains a Python package intended for distribution on PyPI. It provides a convenient utility designed to address longstanding issues related to Korean text encoding in UTF-8, helping developers handle and process Hangul characters more reliably and efficiently.

Each character is converted into:

- 1 byte: 초성 (initial consonant)
- 1 byte: 중성 (vowel)
- 1 byte: 종성 (final consonant)

Missing components are represented as `0x00`.

해당 레포지토리는 PyPI에 올리기 위한 레포입니다. UTF-8의 한글 인코딩에서 발생하는 고질적인 문제점을 해결하여 보다 간편하게 사용하고자 만든 패키지입니다.

한 글자를 초성 중성 종성 - 3바이트로 나누어 표시를 해줍니다. 기존 유니코드 방식또한 3바이트를 사용하나 초성 중성 종성의 관계가 아닌 가-힣 의 순서로 배치되어 비교연산이 어려웠기에 개선하였습니다.

---

## Installation

```bash
pip install hangultrans_bjw
```

or 

```bash
/* 깃 클론 후 */
pip install -v -e .
```

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