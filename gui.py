import customtkinter as ctk
import subprocess
import os

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")


class ScheduleApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Генератор расписания 2.0")
        self.geometry("1100x650")
        self.schedule_data = []

        # --- Левая панель (Меню) ---
        self.sidebar = ctk.CTkFrame(self, width=220, corner_radius=0, fg_color=("gray85", "gray17"))
        self.sidebar.pack(side="left", fill="y")

        self.logo_label = ctk.CTkLabel(self.sidebar, text="📅 Умное расписание",
                                       font=ctk.CTkFont(size=20, weight="bold"))
        self.logo_label.pack(padx=20, pady=(30, 20))

        self.generate_btn = ctk.CTkButton(self.sidebar, text="⚙️ Сгенерировать", height=40,
                                          font=ctk.CTkFont(size=14, weight="bold"), command=self.run_generation)
        self.generate_btn.pack(padx=20, pady=10)

        self.filter_label = ctk.CTkLabel(self.sidebar, text="🔍 Фильтр по группе:", font=ctk.CTkFont(size=14))
        self.filter_label.pack(padx=20, pady=(40, 5))

        self.group_filter = ctk.CTkComboBox(self.sidebar, values=["Все"], width=180, height=35,
                                            command=self.apply_filter)
        self.group_filter.pack(padx=20, pady=5)

        # --- Основная область ---
        self.main_frame = ctk.CTkFrame(self, fg_color="transparent")
        self.main_frame.pack(side="right", fill="both", expand=True, padx=20, pady=20)

        self.status_label = ctk.CTkLabel(self.main_frame, text="Готово к работе. Нажмите «Сгенерировать».",
                                         font=ctk.CTkFont(size=16, slant="italic"))
        self.status_label.pack(pady=(0, 15))

        self.table_frame = ctk.CTkScrollableFrame(self.main_frame, fg_color=("gray90", "gray13"), corner_radius=15)
        self.table_frame.pack(fill="both", expand=True)

    def run_generation(self):
        self.status_label.configure(text="Запуск C++ ядра... Ожидайте.", text_color=("black", "white"))
        self.update()

        exe_name = "x64/Debug/time_table.exe"

        try:
            subprocess.run([f"./{exe_name}", "--auto"], check=True)

            if os.path.exists("schedule.txt"):
                self.load_data("schedule.txt")
                self.status_label.configure(text="Расписание успешно сгенерировано!")
            else:
                self.status_label.configure(text="Ошибка: файл schedule.txt не найден.", text_color="red")
        except Exception as e:
            self.status_label.configure(text="Ошибка при запуске C++ ядра.", text_color="red")

    def load_data(self, filename):
        self.schedule_data = []
        groups = set()

        with open(filename, "r", encoding="utf-8") as f:
            lines = f.readlines()

        row_id = 0  # Сохраняем оригинальный индекс строки для связи с C++
        for line in lines:
            if "|" not in line:
                continue

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
            # Запоминаем индекс вместе с данными
            self.schedule_data.append({"id": row_id, "data": clean_parts})
            row_id += 1

        filter_values = ["Все"] + sorted(list(groups))
        self.group_filter.configure(values=filter_values)

        self.draw_table(self.group_filter.get())

    def apply_filter(self, choice):
        self.draw_table(choice)

    def draw_table(self, group_filter):
        for widget in self.table_frame.winfo_children():
            widget.destroy()

        headers = ["Группа", "Предмет", "Время", "Аудитория", "Преподаватель", "Действие"]
        for col, text in enumerate(headers):
            lbl = ctk.CTkLabel(self.table_frame, text=text, font=ctk.CTkFont(size=14, weight="bold"),
                               fg_color=("gray75", "gray25"), corner_radius=8, height=35)
            lbl.grid(row=0, column=col, padx=4, pady=(0, 10), sticky="ew")

        for i in range(5):
            self.table_frame.grid_columnconfigure(i, weight=1)

        display_row = 1
        for item in self.schedule_data:
            real_id = item["id"]
            row = item["data"]

            if group_filter != "Все" and row[0] != group_filter:
                continue

            # Эффект зебры: чередование цветов строк
            bg_color = "transparent" if display_row % 2 == 0 else ("gray85", "gray20")

            for col, text in enumerate(row):
                lbl = ctk.CTkLabel(self.table_frame, text=text, anchor="w", fg_color=bg_color, corner_radius=4,
                                   height=30)
                lbl.grid(row=display_row, column=col, padx=4, pady=2, sticky="ew")

            btn = ctk.CTkButton(self.table_frame, text="✏️ Изменить", width=90, height=28,
                                fg_color="#1f538d", hover_color="#14375e",
                                command=lambda r_id=real_id, r_data=row: self.open_edit_window(r_id, r_data))
            btn.grid(row=display_row, column=5, padx=4, pady=2)

            display_row += 1

    def open_edit_window(self, row_id, row_data):
        edit_win = ctk.CTkToplevel(self)
        edit_win.title("Ручное редактирование")
        edit_win.geometry("400x350")
        edit_win.attributes("-topmost", True)  # Окно всегда поверх таблицы

        title_lbl = ctk.CTkLabel(edit_win, text=f"{row_data[0]} | {row_data[1]}",
                                 font=ctk.CTkFont(weight="bold", size=16))
        title_lbl.pack(pady=15)

        time_lbl = ctk.CTkLabel(edit_win, text="Новый ID времени (1-30):")
        time_lbl.pack(pady=5)
        time_entry = ctk.CTkEntry(edit_win, justify="center")
        time_entry.pack(pady=5)

        room_lbl = ctk.CTkLabel(edit_win, text="Новый ID аудитории:")
        room_lbl.pack(pady=5)
        room_entry = ctk.CTkEntry(edit_win, justify="center")
        room_entry.pack(pady=5)

        def save_changes():
            new_t = time_entry.get().strip()
            new_r = room_entry.get().strip()

            if not new_t.isdigit() or not new_r.isdigit():
                self.status_label.configure(text="Ошибка: Введите числовые ID.", text_color="orange")
                edit_win.destroy()
                return

            exe_name = "x64/Debug/time_table.exe"
            try:
                # Запускаем C++ без check=True, чтобы прочитать код возврата
                result = subprocess.run([f"./{exe_name}", "--edit", str(row_id), new_t, new_r])

                if result.returncode == 0:
                    self.status_label.configure(text="Успех: Занятие изменено и закреплено!", text_color="green")
                    self.load_data("schedule.txt")  # Обновляем таблицу, читая измененный файл
                elif result.returncode == 2:
                    self.status_label.configure(text="Ошибка: Конфликт! Аудитория или время уже заняты.",
                                                text_color="red")
                else:
                    self.status_label.configure(text="Ошибка: Внутренняя ошибка C++ или неверный индекс.",
                                                text_color="red")

                edit_win.destroy()
            except Exception as e:
                self.status_label.configure(text="Ошибка связи с C++ ядром.", text_color="red")
                edit_win.destroy()

        save_btn = ctk.CTkButton(edit_win, text="Проверить и сохранить", command=save_changes, fg_color="green",
                                 hover_color="darkgreen")
        save_btn.pack(pady=20)


if __name__ == "__main__":
    app = ScheduleApp()
    app.mainloop()