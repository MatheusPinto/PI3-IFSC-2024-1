import os

VALID_TAGS = [
    "lettuce",
    "rice",
    "banana",
    "potato",
    "meat",
    "onion",
    "beans",
    "chicken",
    "orange",
    "milk",
    "apple",
    "watermelon",
    "strawberry",
    "egg",
    "tomato",
]

VALID_MODES = ["test", "train", "validation"]

selected_tag = input("Enter desired folder name to be used:")

if selected_tag not in VALID_TAGS:
    print(
        f"\nThe selected folder name '{selected_tag}' is not valid!! Valid choices are:"
    )
    print(", ".join(VALID_TAGS))
    exit()

selected_mode = input("Enter the folder training mode to be renamed:")
if selected_mode not in VALID_MODES:
    print(f"\nThe selected mode '{selected_mode}' is not valid!! Valid choices are:")
    print(", ".join(VALID_MODES))
    exit()

destination_path = f"/data/images/{selected_mode}/{selected_tag}"
current_dir = os.getcwd()

try:
    os.chdir(current_dir + destination_path)
except Exception:
    print(f'\nthe current directory path "{destination_path}" is not valid!!\n')
    print(
        f"make sure this script is inserted where the following folders for the mode {selected_mode} would be for example: /data/images/{selected_mode}/{selected_tag}"
    )
    exit()

accepted_extensions = ["jpg", "jpeg", "png"]

count = int(
    input("Enter the starting value to be used while renaming the image files:")
)

print(os.getcwd())

for filename in os.listdir(os.getcwd()):
    with open(os.path.join(os.getcwd(), filename), "r") as f:
        name, _, extension = filename.partition(".")

        if extension.lower() not in accepted_extensions:
            continue

        new_filename = f"img_{count}.{extension}"
        os.rename(filename, new_filename)
        count = count + 1
