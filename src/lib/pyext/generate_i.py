import os

def generate_swig_interface(header_file):
    module_name = os.path.splitext(os.path.basename(header_file))[0]
    interface_content = f"%module {module_name}\n\n%{{\n#include \"{header_file}\"\n%}}\n\n%include \"{header_file}\"\n"

    with open(f"{module_name}.i", "w") as interface_file:
        interface_file.write(interface_content)

def generate_swig_interfaces_in_directory(directory):
    header_files = [f for f in os.listdir(directory) if f.endswith(".h")]

    for header_file in header_files:
        generate_swig_interface(os.path.join(directory, header_file))

# Example usage
generate_swig_interfaces_in_directory("/path/to/your/library/headers")
