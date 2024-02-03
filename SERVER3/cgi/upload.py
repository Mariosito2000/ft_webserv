#!/usr/bin/env python3

import os
import cgi
import shutil

def copy_file_to_directory(file_path, destination_directory):
    # Check if the file exists
    if not os.path.exists(file_path):
        print(f"Error: File '{file_path}' does not exist.")
        return

    # Extract the filename and extension from the file path
    file_name = os.path.basename(file_path)
    
    # Create the destination path by combining the destination directory and the file name
    destination_path = os.path.join(destination_directory, file_name)

    try:
        # Copy the file to the destination directory
        shutil.copy2(file_path, destination_path)
        print(f"File copied successfully to {destination_path}")
    except Exception as e:
        print(f"Error copying file: {e}")

# Example usage:
absolute_path = os.environ.get('PATH_INFO', '')
destination_directory = os.getcwd() + "/SERVER1/cgi_upload"

copy_file_to_directory(absolute_path, destination_directory)

