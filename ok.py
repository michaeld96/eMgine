# Let's write a Python script that will parse the hardcoded map
# and generate the corresponding JSON structure for the actors.

hardcoded_map = [
    "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
    "bb    b       b                   b        b    bb",
    "bb            b                   b             bb",
    "bb    bbbbbbbbbbbbbb              bbbbbbbbbb    bb",
    "bb            b      b            b             bb",
    "bb    b       b       b   bb      b        b    bb",
    "bb    b  bb bbb     bb      bb    bbbbbbbbbb    bb",
    "bb    b  b    b   bb          bb           b    bb",
    "bb    b  b   b  bb        bb    bb         b    bb",
    "bbb   bbbb  bbbb                  bbbbbbbbbb   bbb",
    "b                   bb                           b",
    "b                                                b",
    "bbb    bbbb  bbbb          bb    bbbbbbbbbbbb  bbb",
    "bb       b      bbbbb        bbb           b    bb",
    "bb       b            b    b               b    bb",
    "bb       b            b    b               b    bb",
    "bb       b            b    b               b    bb",
    "bb       bbbbbbbbbbbbb      bbbbbbbbbbbbbbbb    bb",
    "bb                                              bb",
    "bb        bb           b  b         bb          bb",
    "bb                    b    b            bb      bb",
    "bbbbb    bb           b    b                   bbb",
    "bb   bb                bbbb      bb      bb      b",
    "bb     bb    bb                      bb          b",
    "bbbbbbbbbbbbbbbb   bbbbbbbbbbbb   bbbbbbbbbbbbbbbb",
    "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
]

# Function to generate JSON structure for the actors from the hardcoded map
def generate_json_from_map(hardcoded_map):
    actors = []
    for y, row in enumerate(hardcoded_map):
        for x, char in enumerate(row):
            if char != ' ':  # Assuming spaces are empty tiles and everything else is an actor
                actor = {
                    "name": f"{char}_{x}_{y}",
                    "view": char,
                    "x": x,
                    "y": y,
                    "vel_x": 0,
                    "vel_y": 0,
                    "blocking": char.lower() == 'b',  # Assuming 'b' is for blocking walls
                    "nearby_dialogue": "",
                    "contact_dialogue": "",
                }
                actors.append(actor)
    scene = {"actors": actors}
    return scene

# Generate the JSON structure
scene_json = generate_json_from_map(hardcoded_map)

# Now we will save this to a JSON file
import json

# Define the output file path
output_file_path = 'scene.json'

# Write the JSON structure to a file
with open(output_file_path, 'w') as json_file:
    json.dump(scene_json, json_file, indent=4)

output_file_path
