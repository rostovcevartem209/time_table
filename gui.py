import customtkinter as ctk
import subprocess
import os

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")


class ScheduleApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Генератор расписания")
        self.geometry("900x600")

        self.sidebar = ctk.CTkFrame(self, width=200, corner_radius=0)
        self.sidebar.pack(side="left", fill="y")

        self.logo_label = ctk.CTkLabel(self.sidebar, text="Меню", font=ctk.CTkFont(size=20, weight="bold"))
        self.logo_label.pack(padx=20, pady=20)

        self.generate_btn = ctk.CTkButton(self.sidebar, text="Сгенерировать", command=self.run_generation)
        self.generate_btn.pack(padx=20, pady=10)

        self.main_frame = ctk.CTkFrame(self)
        self.main_frame.pack(side="right", fill="both", expand=True, padx=10, pady=10)

        self.status_label = ctk.CTkLabel(self.main_frame, text="Нажмите Сгенерировать для запуска алгоритма",
                                         font=ctk.CTkFont(size=16))
        self.status_label.pack(pady=10)

        # Поле для красивого вывода нашего расписания
        self.textbox = ctk.CTkTextbox(self.main_frame, font=ctk.CTkFont(size=14))
        self.textbox.pack(pady=10, padx=10, fill="both", expand=True)

    def run_generation(self):
        self.status_label.configure(text="Запуск C++ ядра... Ожидайте.")
        self.update()  # Принудительно обновляем экран до начала вычислений

        # ИМЯ ФАЙЛА: укажи точное название твоего скомпилированного файла
        exe_name = "x64/Debug/time_table.exe"

        try:
            subprocess.run([f"./{exe_name}", "--auto"], check=True)

            if os.path.exists("schedule.txt"):
                with open("schedule.txt", "r", encoding="utf-8") as f:
                    text = f.read()

                self.textbox.delete("0.0", "end")
                self.textbox.insert("0.0", text)
                self.status_label.configure(text="Расписание успешно сгенерировано!")
            else:
                self.status_label.configure(text="Ошибка: файл schedule.txt не найден.")
        except Exception as e:
            self.status_label.configure(text="Ошибка: Проверьте правильность имени exe файла.")


if __name__ == "__main__":
    app = ScheduleApp()
    app.mainloop()