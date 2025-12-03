import os

def code_to_markdown(folder_path, output_file="code_output.md", extensions=(".c", ".cpp", ".h")):
    """
    Đọc tất cả file .c, .cpp, .h trong thư mục và ghi vào file Markdown.
    
    :param folder_path: Đường dẫn đến thư mục chứa code
    :param output_file: Tên file đầu ra Markdown
    :param extensions: Tuple các đuôi file cần đọc
    """
    with open(output_file, "w", encoding="utf-8") as md_file:
        md_file.write("# Source Code Collection\n\n")
        # md_file.write(f"# server/{folder_path}\n\n")

        for root, dirs, files in os.walk(folder_path):
            for file in files:
                if file.lower().endswith(extensions):
                    file_path = os.path.join(root, file)
                    relative_path = os.path.relpath(file_path, folder_path)

                    md_file.write(f"##  server/src/{relative_path} \n\n")
                    md_file.write("```cpp\n")
                    
                    try:
                        with open(file_path, "r", encoding="utf-8") as f:
                            content = f.read()
                            md_file.write(content)
                    except Exception as e:
                        md_file.write(f"// Lỗi khi đọc file: {e}\n")
                    
                    md_file.write("\n```\n\n---\n\n")

    print(f"✅ Đã xuất mã nguồn ra file: {output_file}")

# === Sử dụng ===
if __name__ == "__main__":
    FOLDER_PATH = "/home/pypy/Projects/NetworkProgramming/server/src"  # Thay bằng đường dẫn thư mục của bạn
    OUTPUT_FILE = "current_server_code.md"
    
    if not os.path.exists(FOLDER_PATH):
        print(f"❌ Thư mục không tồn tại: {FOLDER_PATH}")
    else:
        code_to_markdown(FOLDER_PATH, OUTPUT_FILE)