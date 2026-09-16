import customtkinter as ctk
import subprocess
import os

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")


class ScheduleApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Генератор расписания 2.0")
        self.geometry("1100x700")
        self.schedule_data = []
        self.groups_set = set()
        self.teachers_set = set()

        # --- НАСТРОЙКА СЛОВАРЕЙ ПЕРЕВОДА ---
        self.time_mapping = {}
        days = ["ПН", "ВТ", "СР", "ЧТ", "ПТ"]
        slot_id = 1
        for day in days:
            for para in range(1, 7):
                self.time_mapping[f"{day} {para} пара"] = slot_id
                slot_id += 1

        # --- Левая панель (Меню) ---
        self.sidebar = ctk.CTkFrame(self, width=250, corner_radius=0, fg_color=("gray85", "gray17"))
        self.sidebar.pack(side="left", fill="y")

        self.logo_label = ctk.CTkLabel(self.sidebar, text="📅 Умное расписание",
                                       font=ctk.CTkFont(size=20, weight="bold"))
        self.logo_label.pack(padx=20, pady=(30, 20))

        self.generate_btn = ctk.CTkButton(self.sidebar, text="⚙️ Сгенерировать", height=40,
                                          font=ctk.CTkFont(size=14, weight="bold"), command=self.run_generation)
        self.generate_btn.pack(padx=20, pady=10)

        # НОВАЯ ФИЧА: Переключатель режима поиска
        self.filter_mode = ctk.CTkSegmentedButton(self.sidebar, values=["По группе", "По преподавателю"],
                                                  command=self.change_filter_mode)
        self.filter_mode.pack(padx=20, pady=(30, 10))
        self.filter_mode.set("По группе")

        self.item_filter = ctk.CTkComboBox(self.sidebar, values=["Данные не загружены"], width=200, height=35,
                                           command=self.apply_filter)
        self.item_filter.pack(padx=20, pady=5)

        # --- Основная область ---
        self.main_frame = ctk.CTkFrame(self, fg_color="transparent")
        self.main_frame.pack(side="right", fill="both", expand=True, padx=20, pady=20)

        self.status_label = ctk.CTkLabel(self.main_frame, text="Готово к работе. Нажмите «Сгенерировать».",
                                         font=ctk.CTkFont(size=16, slant="italic"))
        self.status_label.pack(pady=(0, 15))

        self.table_frame = ctk.CTkScrollableFrame(self.main_frame, fg_color=("gray90", "gray13"), corner_radius=15)
        self.table_frame.pack(fill="both", expand=True)

        # --- АВТОЗАГРУЗКА РАСПИСАНИЯ ПРИ СТАРТЕ ---
        if os.path.exists("schedule.txt"):
            try:
                self.load_data("schedule.txt")
                self.status_label.configure(text="✅ Загружено сохраненное расписание!", text_color="green")
            except Exception as e:
                self.status_label.configure(text="⚠️ Ошибка чтения schedule.txt, сгенерируйте новое.",
                                            text_color="orange")

    def run_generation(self):
        self.status_label.configure(text="⏳ Запуск C++ ядра... Ожидайте.", text_color=("black", "white"))
        self.update()

        exe_name = "x64/Debug/time_table.exe"

        try:
            subprocess.run([f"./{exe_name}", "--auto"], check=True)

            if os.path.exists("schedule.txt"):
                self.load_data("schedule.txt")
                self.status_label.configure(text="✅ Расписание успешно сгенерировано!", text_color="green")
            else:
                self.status_label.configure(text="❌ Ошибка: файл schedule.txt не найден.", text_color="red")
        except Exception as e:
            self.status_label.configure(text="❌ Ошибка при запуске C++ ядра.", text_color="red")

    def load_data(self, filename):
        self.schedule_data = []
        self.groups_set.clear()
        self.teachers_set.clear()

        with open(filename, "r", encoding="utf-8") as f:
            lines = f.readlines()

        row_id = 0
        for line in lines:
            if "|" not in line:
                continue

            parts = line.strip().split("|")
            group_name = parts[0].replace("Группа ", "").strip()
            teacher_name = parts[4].replace("Преподаватель ", "").strip()

            self.groups_set.add(group_name)
            self.teachers_set.add(teacher_name)

            clean_parts = [
                group_name,
                parts[1].replace("Предмет ", "").strip(),
                parts[2].replace("Время ", "").strip(),
                parts[3].replace("Аудитория ", "").strip(),
                teacher_name
            ]
            self.schedule_data.append({"id": row_id, "data": clean_parts})
            row_id += 1

        self.schedule_data.sort(key=lambda x: self.time_mapping.get(x["data"][2], 999))
        self.change_filter_mode(self.filter_mode.get())

    def change_filter_mode(self, mode):
        if not self.schedule_data:
            return

        if mode == "По группе":
            values = ["Выберите группу..."] + sorted(list(self.groups_set))
        else:
            values = ["Выберите преподавателя..."] + sorted(list(self.teachers_set))

        self.item_filter.configure(values=values)
        self.item_filter.set(values[0])
        self.draw_table(values[0], mode)

    def apply_filter(self, choice):
        self.draw_table(choice, self.filter_mode.get())

    def draw_table(self, filter_value, mode):
        for widget in self.table_frame.winfo_children():
            widget.destroy()

        if "Выберите" in filter_value or "Данные" in filter_value:
            lbl = ctk.CTkLabel(self.table_frame, text="👈 Выберите нужный пункт в меню слева",
                               font=ctk.CTkFont(size=16, slant="italic"), text_color="gray")
            lbl.pack(pady=50)
            return

        # Если смотрим группу - показываем препода, если смотрим препода - показываем группу
        dynamic_header = "Преподаватель" if mode == "По группе" else "Группа"
        headers = ["Предмет", "Время", "Аудитория", dynamic_header, "Действие"]

        for col in range(len(headers)):
            self.table_frame.grid_columnconfigure(col, weight=1)

        for col, text in enumerate(headers):
            lbl = ctk.CTkLabel(self.table_frame, text=text, font=ctk.CTkFont(size=14, weight="bold"),
                               fg_color=("gray75", "gray25"), corner_radius=8, height=35)
            lbl.grid(row=0, column=col, padx=4, pady=(0, 10), sticky="ew")

        display_row = 1
        current_day = ""

        for item in self.schedule_data:
            row = item["data"]

            # Проверяем совпадение по группе или по преподавателю
            if (mode == "По группе" and row[0] != filter_value) or \
                    (mode == "По преподавателю" and row[4] != filter_value):
                continue

            time_str = row[2]
            day = time_str.split(" ")[0] if " " in time_str else ""

            if day != current_day:
                current_day = day
                day_lbl = ctk.CTkLabel(self.table_frame, text=f"==== {current_day} ====",
                                       font=ctk.CTkFont(size=14, weight="bold"), text_color=("#1f538d", "#569cd6"))
                day_lbl.grid(row=display_row, column=0, columnspan=5, pady=(15, 5))
                display_row += 1

            real_id = item["id"]

            # ЦВЕТОВАЯ КОДИРОВКА КОРПУСОВ (Г - синий оттенок, БМ - зеленый оттенок)
            room = row[3]
            if "Г-" in room:
                room_color = ("#d0e2ff", "#1c3b6b")  # Мягкий синий
            elif "БМ-" in room:
                room_color = ("#d4edda", "#1e4620")  # Мягкий зеленый
            else:
                room_color = "transparent"

            bg_color = "transparent" if display_row % 2 == 0 else ("gray85", "gray20")

            # Выбираем, какие данные отображать в зависимости от режима
            dynamic_value = row[4] if mode == "По группе" else row[0]
            display_data = [row[1], row[2], row[3], dynamic_value]

            for col, text in enumerate(display_data):
                lbl_color = room_color if col == 2 else bg_color
                lbl = ctk.CTkLabel(self.table_frame, text=text, anchor="w", fg_color=lbl_color, corner_radius=4,
                                   height=30)
                lbl.grid(row=display_row, column=col, padx=4, pady=2, sticky="ew")

            btn = ctk.CTkButton(self.table_frame, text="✏️ Изменить", width=90, height=28,
                                fg_color="#1f538d", hover_color="#14375e",
                                command=lambda r_id=real_id, r_data=row: self.open_edit_window(r_id, r_data))
            btn.grid(row=display_row, column=4, padx=4, pady=2)

            display_row += 1

    def open_edit_window(self, row_id, row_data):
        edit_win = ctk.CTkToplevel(self)
        edit_win.title("Редактирование занятия")
        edit_win.geometry("450x380")
        edit_win.attributes("-topmost", True)

        title_lbl = ctk.CTkLabel(edit_win, text=f"{row_data[0]} | {row_data[1]}",
                                 font=ctk.CTkFont(weight="bold", size=18))
        title_lbl.pack(pady=(20, 15))

        time_lbl = ctk.CTkLabel(edit_win, text="Выберите новое время:", font=ctk.CTkFont(size=14))
        time_lbl.pack(pady=(10, 5))
        time_combo = ctk.CTkComboBox(edit_win, values=list(self.time_mapping.keys()), width=200)
        time_combo.pack(pady=5)

        if row_data[2] in self.time_mapping:
            time_combo.set(row_data[2])

        room_lbl = ctk.CTkLabel(edit_win, text="Выберите новую аудиторию:", font=ctk.CTkFont(size=14))
        room_lbl.pack(pady=(10, 5))
        room_combo = ctk.CTkComboBox(edit_win, values=list(self.room_mapping.keys()), width=200)
        room_combo.pack(pady=5)

        if row_data[3] in self.room_mapping:
            room_combo.set(row_data[3])

        def save_changes():
            selected_time_name = time_combo.get()
            selected_room_name = room_combo.get()

            new_t_id = str(self.time_mapping.get(selected_time_name, -1))
            new_r_id = str(self.room_mapping.get(selected_room_name, -1))

            exe_name = "x64/Debug/time_table.exe"
            try:
                result = subprocess.run([f"./{exe_name}", "--edit", str(row_id), new_t_id, new_r_id])

                if result.returncode == 0:
                    self.status_label.configure(text="✅ Успех: Занятие изменено и закреплено!", text_color="green")
                    self.load_data("schedule.txt")
                elif result.returncode == 2:
                    self.status_label.configure(text="❌ Ошибка: Конфликт! Аудитория занята или не подходит.",
                                                text_color="red")
                else:
                    self.status_label.configure(text="❌ Ошибка: Неверные данные или сбой C++.", text_color="red")

                edit_win.destroy()
            except Exception as e:
                self.status_label.configure(text="❌ Ошибка связи с C++ ядром.", text_color="red")
                edit_win.destroy()

        save_btn = ctk.CTkButton(edit_win, text="💾 Сохранить", command=save_changes, fg_color="#28a745",
                                 hover_color="#218838", font=ctk.CTkFont(weight="bold"))
        save_btn.pack(pady=(25, 10))


if __name__ == "__main__":
    app = ScheduleApp()
    app.mainloop()