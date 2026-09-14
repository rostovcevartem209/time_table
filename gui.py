import customtkinter as ctk
import subprocess
import os

# настройка темы
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")


class ScheduleApp(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.title("Генератор расписания")
        self.geometry("900x600")

        # левая панель управления
        self.sidebar = ctk.CTkFrame(self, width=200, corner_radius=0)
        self.sidebar.pack(side="left", fill="y")

        self.logo_label = ctk.CTkLabel(self.sidebar, text="Меню", font=ctk.CTkFont(size=20, weight="bold"))
        self.logo_label.pack(padx=20, pady=20)

        self.generate_btn = ctk.CTkButton(self.sidebar, text="Сгенерировать", command=self.run_generation)
        self.generate_btn.pack(padx=20, pady=10)

        # основная область для таблиц
        self.main_frame = ctk.CTkFrame(self)
        self.main_frame.pack(side="right", fill="both", expand=True, padx=10, pady=10)

        self.status_label = ctk.CTkLabel(self.main_frame, text="Нажмите Сгенерировать для запуска алгоритма",
                                         font=ctk.CTkFont(size=16))
        self.status_label.pack(pady=20)

    def run_generation(self):
        self.status_label.configure(text="Запуск C++ ядра ожидайте")

        # здесь мы позже добавим скрытый запуск скомпилированного файла
        # subprocess.run(["./main"], capture_output=True)

        self.status_label.configure(text="Расписание готово Данные загружены")


if __name__ == "__main__":
    app = ScheduleApp()
    app.mainloop()