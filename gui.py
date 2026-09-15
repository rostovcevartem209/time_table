import customtkinter as ctk
import subprocess
import os

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")


class ScheduleApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Генератор расписания")
        self.geometry("1000x600")  # Сделали окно чуть шире для таблицы

        # Левая панель
        self.sidebar = ctk.CTkFrame(self, width=200, corner_radius=0)
        self.sidebar.pack(side="left", fill="y")

        self.logo_label = ctk.CTkLabel(self.sidebar, text="Меню", font=ctk.CTkFont(size=20, weight="bold"))
        self.logo_label.pack(padx=20, pady=20)

        self.generate_btn = ctk.CTkButton(self.sidebar, text="Сгенерировать", command=self.run_generation)
        self.generate_btn.pack(padx=20, pady=10)

        # Основная область
        self.main_frame = ctk.CTkFrame(self)
        self.main_frame.pack(side="right", fill="both", expand=True, padx=10, pady=10)

        self.status_label = ctk.CTkLabel(self.main_frame, text="Нажмите Сгенерировать для запуска алгоритма",
                                         font=ctk.CTkFont(size=16))
        self.status_label.pack(pady=10)

        # Прокручиваемый фрейм вместо простого текстового поля
        self.table_frame = ctk.CTkScrollableFrame(self.main_frame)
        self.table_frame.pack(pady=10, padx=10, fill="both", expand=True)

    def run_generation(self):
        self.status_label.configure(text="Запуск C++ ядра... Ожидайте.")
        self.update()

        exe_name = "x64/Debug/time_table.exe"

        try:
            subprocess.run([f"./{exe_name}", "--auto"], check=True)

            if os.path.exists("schedule.txt"):
                self.parse_and_draw_table("schedule.txt")
                self.status_label.configure(text="Расписание успешно сгенерировано!")
            else:
                self.status_label.configure(text="Ошибка: файл schedule.txt не найден.")
        except Exception as e:
            self.status_label.configure(text="Ошибка при запуске C++ ядра.")

    def parse_and_draw_table(self, filename):
        # Очищаем таблицу перед новой отрисовкой
        for widget in self.table_frame.winfo_children():
            widget.destroy()

        # Заголовки столбцов
        headers = ["Группа", "Предмет", "Время", "Аудитория", "Преподаватель"]
        for col, text in enumerate(headers):
            lbl = ctk.CTkLabel(self.table_frame, text=text, font=ctk.CTkFont(weight="bold"),
                               fg_color=("gray75", "gray30"), corner_radius=6)
            lbl.grid(row=0, column=col, padx=5, pady=5, sticky="ew")

        # Настраиваем ширину столбцов
        for i in range(5):
            self.table_frame.grid_columnconfigure(i, weight=1)

        # Читаем файл и заполняем строки
        with open(filename, "r", encoding="utf-8") as f:
            lines = f.readlines()

        # Пропускаем первую строку с разделителями, если она есть
        row_index = 1
        for line in lines:
            if "|" not in line:
                continue

            # Убираем лишние слова вроде "Группа", "Предмет" оставляя только суть
            parts = line.strip().split("|")
            clean_parts = [
                parts[0].replace("Группа ", "").strip(),
                parts[1].replace("Предмет ", "").strip(),
                parts[2].replace("Время ", "").strip(),
                parts[3].replace("Аудитория ", "").strip(),
                parts[4].replace("Преподаватель ", "").strip()
            ]

            for col, text in enumerate(clean_parts):
                lbl = ctk.CTkLabel(self.table_frame, text=text, anchor="w")
                lbl.grid(row=row_index, column=col, padx=5, pady=2, sticky="w")

            row_index += 1


if __name__ == "__main__":
    app = ScheduleApp()
    app.mainloop()