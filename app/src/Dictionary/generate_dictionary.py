#!/usr/bin/env python3
"""
Dictionary Generator for ZMK
Reads words from English.txt and distributes them into dict_[letter].h files
"""

import os
import string

def create_header_file(letter, words):
    """Create a dictionary header file for a specific letter"""
    letter_upper = letter.upper()
    letter_lower = letter.lower()
    
    # Sort words alphabetically
    sorted_words = sorted(words, key=str.lower)
    
    # Create header content
    header_content = f"""#ifndef DICT_{letter_upper}_H
#define DICT_{letter_upper}_H

static const char* const dict_{letter_lower}[] = {{
"""
    
    # Add each word
    for word in sorted_words:
        header_content += f'    "{word}",\n'
    
    # Close the array and add size macro
    header_content += f"""}};\n
#define DICT_{letter_upper}_SIZE (sizeof(dict_{letter_lower}) / sizeof(dict_{letter_lower}[0]))

#endif // DICT_{letter_upper}_H
"""
    
    return header_content

def distribute_words_by_letter(words):
    """Distribute words into dictionaries by first letter"""
    letter_dict = {}
    
    # Initialize dictionaries for each letter
    for letter in string.ascii_lowercase:
        letter_dict[letter] = []
    
    # Distribute words
    for word in words:
        if word and len(word) > 0:
            first_letter = word[0].lower()
            if first_letter in letter_dict:
                letter_dict[first_letter].append(word)
            else:
                # Handle non-alphabetic characters (put in 'a' for now)
                letter_dict['a'].append(word)
    
    return letter_dict

def load_words_from_file(filepath):
    """Load words from the English.txt file"""
    print(f"Loading words from {filepath}...")
    
    if not os.path.exists(filepath):
        print(f"Error: File not found: {filepath}")
        return []
    
    words = []
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                word = line.strip()
                if word:  # Skip empty lines
                    words.append(word)
    except Exception as e:
        print(f"Error reading file: {e}")
        return []
    
    print(f"Loaded {len(words):,} words")
    return words

def generate_dictionary_files(words, output_dir):
    """Generate all dictionary header files"""
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Distribute words by letter
    letter_dict = distribute_words_by_letter(words)
    
    total_words = 0
    total_size = 0
    
    print("\nGenerating dictionary files...")
    
    # Create header file for each letter
    for letter in string.ascii_lowercase:
        words_for_letter = letter_dict[letter]
        
        if words_for_letter:
            # Create header content
            header_content = create_header_file(letter, words_for_letter)
            
            # Write to file
            filename = f"dict_{letter}.h"
            filepath = os.path.join(output_dir, filename)
            
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(header_content)
            
            file_size = os.path.getsize(filepath)
            total_words += len(words_for_letter)
            total_size += file_size
            
            print(f"  {filename}: {len(words_for_letter):,} words, {file_size:,} bytes")
        else:
            print(f"  dict_{letter}.h: No words (skipped)")
    
    return total_words, total_size

def main():
    print("ZMK Dictionary Generator")
    print("=" * 40)
    
    # File paths
    input_file = "English_with_contractions.txt"
    if not os.path.exists(input_file):
        input_file = "English.txt"
        if not os.path.exists(input_file):
            print("Error: No English dictionary file found!")
            print("Looking for: English_with_contractions.txt or English.txt")
            return
    
    output_dir = "."  # Current directory
    
    # Load words
    words = load_words_from_file(input_file)
    if not words:
        return
    
    # Generate dictionary files
    total_words, total_size = generate_dictionary_files(words, output_dir)
    
    # Calculate flash usage (assuming 700KB available)
    flash_usage = (total_size / (700 * 1024)) * 100
    
    print("\n" + "=" * 40)
    print("Dictionary Generation Complete!")
    print(f"Total words: {total_words:,}")
    print(f"Total size: {total_size:,} bytes ({total_size/1024:.1f} KB)")
    print(f"Flash usage: {flash_usage:.1f}% of 700KB available")
    print(f"Files generated in: {os.path.abspath(output_dir)}")
    
    # Show distribution
    print("\nWord distribution by letter:")
    words_per_letter = distribute_words_by_letter(words)
    for letter in string.ascii_lowercase:
        count = len(words_per_letter[letter])
        if count > 0:
            print(f"  {letter.upper()}: {count:,} words")

if __name__ == "__main__":
    main()