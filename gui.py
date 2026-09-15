import customtkinter as ctk
import subprocess
import os

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")


class ScheduleApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Генератор расписания")
        self.geometry("1000x600")

        # Здесь будем хранить все распарсенные строки
        self.schedule_data = []

        # --- Левая панель ---
        self.sidebar = ctk.CTkFrame(self, width=200, corner_radius=0)
        self.sidebar.pack(side="left", fill="y")

        self.logo_label = ctk.CTkLabel(self.sidebar, text="Меню", font=ctk.CTkFont(size=20, weight="bold"))
        self.logo_label.pack(padx=20, pady=20)

        self.generate_btn = ctk.CTkButton(self.sidebar, text="Сгенерировать", command=self.run_generation)
        self.generate_btn.pack(padx=20, pady=10)

        # Новый блок фильтрации
        self.filter_label = ctk.CTkLabel(self.sidebar, text="Фильтр по группе:", font=ctk.CTkFont(size=14))
        self.filter_label.pack(padx=20, pady=(30, 5))

        self.group_filter = ctk.CTkComboBox(self.sidebar, values=["Все"], command=self.apply_filter)
        self.group_filter.pack(padx=20, pady=5)

        # --- Основная область ---
        self.main_frame = ctk.CTkFrame(self)
        self.main_frame.pack(side="right", fill="both", expand=True, padx=10, pady=10)

        self.status_label = ctk.CTkLabel(self.main_frame, text="Нажмите Сгенерировать для запуска алгоритма",
                                         font=ctk.CTkFont(size=16))
        self.status_label.pack(pady=10)

        self.table_frame = ctk.CTkScrollableFrame(self.main_frame)
        self.table_frame.pack(pady=10, padx=10, fill="both", expand=True)

    def run_generation(self):
        self.status_label.configure(text="Запуск C++ ядра... Ожидайте.")
        self.update()

        exe_name = "x64/Debug/time_table.exe"

        try:
            subprocess.run([f"./{exe_name}", "--auto"], check=True)

            if os.path.exists("schedule.txt"):
                self.load_data("schedule.txt")
                self.status_label.configure(text="Расписание успешно сгенерировано!")
            else:
                self.status_label.configure(text="Ошибка: файл schedule.txt не найден.")
        except Exception as e:
            self.status_label.configure(text="Ошибка при запуске C++ ядра.")

    def load_data(self, filename):
        self.schedule_data = []
        groups = set()  # Используем set, чтобы группы не повторялись

        with open(filename, "r", encoding="utf-8") as f:
            lines = f.readlines()

        for line in lines:
            if "|" not in line:
                continue

            # Чистим текст
            parts = line.strip().split("|")
            group_name = parts[0].replace("Группа ", "").strip()
            groups.add(group_name)

            clean_parts = [
                group_name,
                parts[1].replace("Предмет ", "").strip(),
                parts[2].replace("Время ", "").strip(),
                parts[3].replace("Аудитория ", "").strip(),
                parts[4].replace("Преподаватель ", "").strip()
            ]
            self.schedule_data.append(clean_parts)

        # Обновляем выпадающий список
        filter_values = ["Все"] + sorted(list(groups))
        self.group_filter.configure(values=filter_values)
        self.group_filter.set("Все")

        # Отрисовываем всё сразу
        self.draw_table("Все")

    def apply_filter(self, choice):
        # Эта функция вызывается при выборе новой группы в ComboBox
        self.draw_table(choice)

    def draw_table(self, group_filter):
        # Очищаем старую таблицу
        for widget in self.table_frame.winfo_children():
            widget.destroy()

        # Рисуем заголовки
        headers = ["Группа", "Предмет", "Время", "Аудитория", "Преподаватель"]
        for col, text in enumerate(headers):
            lbl = ctk.CTkLabel(self.table_frame, text=text, font=ctk.CTkFont(weight="bold"),
                               fg_color=("gray75", "gray30"), corner_radius=6)
            lbl.grid(row=0, column=col, padx=5, pady=5, sticky="ew")

        for i in range(5):
            self.table_frame.grid_columnconfigure(i, weight=1)

        # Заполняем данными
        row_index = 1
        for row in self.schedule_data:
            # Отсеиваем лишнее, если выбран конкретный фильтр
            if group_filter != "Все" and row[0] != group_filter:
                continue

            for col, text in enumerate(row):
                lbl = ctk.CTkLabel(self.table_frame, text=text, anchor="w")
                lbl.grid(row=row_index, column=col, padx=5, pady=2, sticky="w")

            row_index += 1


if __name__ == "__main__":
    app = ScheduleApp()
    app.mainloop()