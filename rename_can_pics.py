import os
from PIL import Image

pics = os.listdir('sources/can_pics')
num_pics = len(pics)
names = [f'can_{str(i).zfill(4)}.png' for i in range(num_pics)]

for i in range(num_pics):
    try:
        with Image.open(f'sources/can_pics/{pics[i]}') as img:
            resized = img.resize((512, 512))
            resized.save(f'sources/can_pics/{pics[i]}')
            os.rename(f'sources/can_pics/{pics[i]}', f'sources/can_pics/{names[i]}')
    except FileNotFoundError:
        print(f"The file {pics[i]} does not exist.")
    except PermissionError:
        print("You do not have permission to rename this file.")
    except Exception as e:
        print(f"An error occurred: {e}")