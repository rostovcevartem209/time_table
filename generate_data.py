import random

NUM_GROUPS = 50
NUM_TEACHERS = 80
NUM_ROOMS = 100


def generate_files():
    # 1. ГРУППЫ (Студентов от 20 до 30)
    groups = [f"43{i:02d}" for i in range(1, NUM_GROUPS + 1)]
    with open("groups.txt", "w", encoding="utf-8") as f:
        for i, name in enumerate(groups):
            students_count = random.randint(20, 30)
            f.write(f"{i} {name} {students_count}\n")

    # 2. ДИСЦИПЛИНЫ
    disciplines = [
        "Программирование", "Высшая_математика", "Физика",
        "Базы_данных", "Схемотехника", "Сети_ЭВМ",
        "Инженерная_графика", "ОС", "Алгоритмы_и_структуры_данных",
        "ТАУ", "Микропроцессорные_системы", "Защита_информации",
        "Экономика", "Философия", "Английский_язык"
    ]
    with open("disciplines.txt", "w", encoding="utf-8") as f:
        for i, name in enumerate(disciplines):
            d_type = i % 3  # Гарантируем, что предметов каждого типа поровну
            f.write(f"{i} {name} {d_type}\n")

    # 3. ПРЕПОДАВАТЕЛИ (Равномерное распределение по предметам)
    surnames = ["Иванов", "Петров", "Смирнов", "Кузнецов", "Попов",
                "Соколов", "Лебедев", "Козлов", "Новиков", "Морозов",
                "Волков", "Алексеев", "Степанов", "Николаев", "Орлов",
                "Павлов", "Макаров", "Захаров", "Зайцев", "Борисов"]
    with open("teachers.txt", "w", encoding="utf-8") as f:
        for i in range(NUM_TEACHERS):
            # ГАРАНТИЯ: Каждый предмет ведет одинаковое число преподавателей
            disc_id = i % len(disciplines)

            # У 80% преподавателей нет выходных, у 20% - один случайный выходной
            unavail = -1
            if random.random() > 0.8:
                unavail = random.randint(0, 4)

            name = f"{surnames[i % len(surnames)]}_{i}"
            f.write(f"{i} {name} {disc_id} {unavail}\n")

    # 4. АУДИТОРИИ (Равномерное распределение типов, без "каморок")
    with open("rooms.txt", "w", encoding="utf-8") as f:
        for i in range(NUM_ROOMS):
            building_prefix = random.choice(["БМ-", "Г-"])
            a = random.randint(1, 5)
            b = random.randint(1, 5)
            d = random.randint(1, 30)
            name = f"{building_prefix}{a}{b}-{d:02d}"

            # ГАРАНТИЯ: Любая аудитория вмещает любую группу (минимум 30)
            capacity = random.choice([30, 60, 100])
            # ГАРАНТИЯ: Хватает всех типов аудиторий (компьютерных, лекционных)
            room_type = i % 3
            f.write(f"{i} {name} {capacity} {room_type}\n")

    # 5. УЧЕБНЫЙ ПЛАН (Объем остается огромным!)
    total_classes = 0
    with open("plans.txt", "w", encoding="utf-8") as f:
        for group_id in range(NUM_GROUPS):
            for disc_id in range(len(disciplines)):
                # Оставляем ту же высокую плотность нагрузки
                if random.random() > 0.4:
                    req_count = random.randint(1, 2)
                    f.write(f"{group_id} {disc_id} {req_count}\n")
                    total_classes += req_count

    print(f"✅ Сгенерирован сбалансированный датасет! Общий объем: {total_classes} занятий.")


if __name__ == "__main__":
    generate_files()