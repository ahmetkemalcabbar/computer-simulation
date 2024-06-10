import simpy
import random
import pandas as pd

PROCESSING_TIMES = {
    'loading': 5,
    'machining': 10,
    'assembling': 8,
    'inspecting': 6,
    'packaging': 4
}
MAINTENANCE_TIME = 3
BREAKDOWN_RATE = 0.1

NUM_WORKERS = {
    'loading': 2,
    'machining': 3,
    'assembling': 4,
    'inspecting': 2,
    'packaging': 3
}
SHIFT_LENGTH = 8
SIMULATION_TIME = 100
NUM_PRODUCTS = 2
class ManufacturingLine:
    def _init_(self, env):
        self.env = env
        self.stages = {
            stage: simpy.Resource(env, capacity=NUM_WORKERS[stage]) for stage in NUM_WORKERS
        }
        self.repair_team = simpy.Resource(env, capacity=2)
        self.data = []

    def process_part(self, part, product_type):
        stages = ['loading', 'machining', 'assembling', 'inspecting', 'packaging']
        for stage in stages:
            processing_time = PROCESSING_TIMES[stage] * (1 if product_type == 1 else 1.2)
            with self.stages[stage].request() as request:
                start_time = self.env.now
                yield request
                try:
                    yield self.env.timeout(processing_time)
                    if random.random() < BREAKDOWN_RATE:
                        yield self.env.process(self.repair_machine(stage))
                    finish_time = self.env.now
                    self.data.append({
                        'Part': part,
                        'Stage': stage,
                        'Start Time': start_time,
                        'Finish Time': finish_time,
                        'Duration': finish_time - start_time,
                        'Product Type': product_type
                    })
                except simpy.Interrupt:
                    # Handle a breakdown during processing
                    yield self.env.process(self.repair_machine(stage))

    def repair_machine(self, stage):
        with self.repair_team.request() as repair:
            start_repair = self.env.now
            yield repair
            yield self.env.timeout(MAINTENANCE_TIME)

def part_manufacturer(env, line, part_id, product_type):
    yield env.process(line.process_part(part_id, product_type))

def setup(env, num_parts_per_type):
    line = ManufacturingLine(env)
    for product_type in range(1, NUM_PRODUCTS + 1):
        for i in range(num_parts_per_type):
            env.process(part_manufacturer(env, line, f"Part_{i + 1}", product_type))

    yield env.timeout(SIMULATION_TIME)
    df = pd.DataFrame(line.data)
    return df

    
env = simpy.Environment()
process = env.process(setup(env, 5))
env.run()
df = process.value
print(df)