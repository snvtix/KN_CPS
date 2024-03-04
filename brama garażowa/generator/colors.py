ALPHABET = "0123456789ABCDEF"

m = [
    '999D9D99D9DDDDD9D9DDDD99D9D99D9D9D99999D99DD',
    '2A2BAAA2BBBBB2A3BBA3B3B23A2332AA2B2BB22233A3',
    '66444644444464466646666666444644664446464666',
    'DD99999D9D9999999999D99D9DDDD99D9D99DD9DDDD9',
    'BA2332222BA3A2A32BAB3B3B23A3233233ABBAA32AB3',
    '46466646646466444646664446664646666446664444',
    'DDDDDD999D9DD99999D99D9999D99D9DD9D9999DDD99',
    'ABAAA2BBBBBBA2A322333AABA2A3B222ABA232AAB33B',
    '44466664646466464664446646666664446646644446',
    'D999D9D999D999D9D99D999D9DD999D99DDD99DDD999',
    'B2B2323AB3222AA2B2BB3A2B32ABA3B3BAB32A23333A',
    '44646666466666464444646464464666444466464464',
    'D99D9DD9D99DDD99DD999D9D9DD9DD999DDDD9DD999D',
    'AABA3B2333333BABBB3B3BABABBBAB2B332A32BA32AB',
    '66644446646466666666646466446446446444646646',
    '999DD9DD99DD999D9DDD9D9999D999D99D99D9D9DD9D',
    '23BBB3A222A2B3A223BA33223333ABAAB3B3BBBB2BBB',
    '66446446646466444664446466646646464446644644',
    '9DD9D9DDDD999DD9D999D9D9999DDDD9DDDD9999999D',
    'AABAB3A23B223B332A2BAB22AA2BAB3BB33322222AAA',
    '46464446464464444446666664644646664666446466',
    '9D9D9999DD999DDDDDD9999D9D999DD99D9D9D99D999',
    'BB3A3B3323322BA3A22BB32B3A2A223233B33BBABB2A',
    '66666446646666646644466446664666646644644664',
    '99999999999999999999999999999999999999999999'
]

# get dimensions of m
m_height = len(m)
m_width = len(m[0])

import random

# create a second matrix, but with random strings

r = []
for y in range(m_height):
    r.append([])
    for x in range(m_width):
        r[y].append(random.choice(ALPHABET))

color_mapping = {}

for letter in ALPHABET:
    color_mapping[letter] = random.randint(0, 0xFFFFFF)


def get_color(letter):
    return color_mapping[letter]


# create a new image with the same dimensions as m
from PIL import Image

img = Image.new("RGB", (m_width, m_height), "black")  # create a new black image
pixels = img.load()  # create the pixel map

# iterate over the image and set the pixels accordingly
for y in range(m_height):
    for x in range(m_width):
        pixels[x, y] = get_color(m[y][x])

# upscale the image by 10x
img = img.resize((m_width * 60, m_height * 60), Image.NEAREST)

# save the image
img.save("recorded.png")

img = Image.new("RGB", (m_width, m_height), "black")  # create a new black image
pixels = img.load()  # create the pixel map


# iterate over the image and set the pixels accordingly
for y in range(m_height):
    for x in range(m_width):
        pixels[x, y] = get_color(r[y][x])

# upscale the image by 10x
img = img.resize((m_width * 60, m_height * 60), Image.NEAREST)

# save the image
img.save("random.png")
