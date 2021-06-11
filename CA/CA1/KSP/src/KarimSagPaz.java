import java.util.ArrayList;
import java.util.Scanner;
import java.util.concurrent.Semaphore;

import static java.lang.Thread.sleep;

class Ingredient {
    String name;
    int type;

    public Ingredient(int type) {
        switch (type) {
            case 1:
                name = "Goosht";
                break;
            case 2:
                name = "Goje";
                break;
            case 3:
                name = "Piaz";
                break;
            case 4:
                name = "Ketchap";
                break;
            case 5:
                name = "Mustard";
                break;
        }
    }
}

class Order {
    int type;
    boolean done;
    int number;

    public Order(int type, boolean done, int number) {
        this.type = type;
        this.done = done;
        this.number = number;
    }
}

class Q {
    // an item
    ArrayList<Ingredient> ingredients = new ArrayList<>();
    ArrayList<Order> orders = new ArrayList<>();
    int num_of_goosht = 2, num_of_goje = 2, num_of_piaz = 2, num_of_ketchap = 2, num_of_mustard = 2;
    int counter = 1;
    int ongoing_time = 0;
    public Q(){
        for (int i = 1; i <= 5; i++) {
            ingredients.add(new Ingredient(i));
            ingredients.add(new Ingredient(i));
        }
    }

    static Semaphore semCon = new Semaphore(1);

    static Semaphore semProd = new Semaphore(1);


    void removeIngredient(int type) {
        for (int i = 0; i < ingredients.size(); i++) {
            if (ingredients.get(i).type == type) {
                ingredients.remove(i);
                break;
            }
        }
    }
    // to get an item from buffer
    void useIngredient(Consumer consumer) {

        //System.out.println("charrom: " + consumer.type);
        try {
            // Before consumer can consume an item,
            // it must acquire a permit from semCon
            semCon.acquire();
            //System.out.println("panjom: " + consumer.type);
        }
        catch (InterruptedException e) {
            System.out.println("InterruptedException caught");
        }

        // consumer consuming an item
        if (consumer.type == 1) { //Gordon Ramsay
            if (num_of_goosht >= 1 && num_of_piaz >= 1 && num_of_goje >= 2 && num_of_ketchap >= 2) {
                removeIngredient(1);
                removeIngredient(2);removeIngredient(2);
                removeIngredient(3);
                removeIngredient(4);removeIngredient(4);
                num_of_goosht--; num_of_piaz--; num_of_goje-=2; num_of_ketchap-=2;
                int orderNumber = 0;
                for (int i = 0; i < orders.size(); i++) {
                    if (orders.get(i).type == 1 && !orders.get(i).done) {
                        orderNumber = orders.get(i).number;
                        orders.get(i).done = true;
                        break;
                    }
                }
                System.out.println(counter + " - " + orderNumber + " - Gordon Ramsay - " + ongoing_time);
                counter++;
                consumer.num_of_clients--;
            }
        }
        else { //Jamie Oliver
            if (num_of_goosht >= 2 && num_of_piaz >= 3 && num_of_mustard >= 2 && num_of_ketchap >= 2) {
                removeIngredient(1);removeIngredient(1);
                removeIngredient(3);removeIngredient(3);removeIngredient(3);
                removeIngredient(4);removeIngredient(4);
                removeIngredient(5);removeIngredient(5);
                num_of_goosht-=2; num_of_piaz-=3; num_of_mustard-=2; num_of_ketchap-=2;
                int orderNumber = 0;
                for (int i = 0; i < orders.size(); i++) {
                    if (orders.get(i).type == 2 && !orders.get(i).done) {
                        orderNumber = orders.get(i).number;
                        orders.get(i).done = true;
                        break;
                    }
                }
                System.out.println(counter + " - " + orderNumber + " - Jamie Oliver - " + ongoing_time);
                counter++;
                consumer.num_of_clients--;
            }
        }

        // After consumer consumes the item,
        // it releases semProd to notify producer
        //System.out.println("shishm: " + consumer.type);
        semProd.release();
    }

    // to put an item in buffer
    void makeIngredient(int ingredientType) {
        try {
            // Before producer can produce an item,
            // it must acquire a permit from semProd

            semProd.acquire();
        }
        catch (InterruptedException e) {
            System.out.println("InterruptedException caught");
        }

        if (ingredients.get(ingredients.size()-1).type == ingredientType) {
            if (ingredientType == 5)
                ingredientType = 1;
            else
                ingredientType++;
        }

        boolean isOk = false;
        switch (ingredientType) {
            case 1:
                if (num_of_goosht < 10)
                    isOk = true;
                break;
            case 2:
                if (num_of_goje < 10)
                    isOk = true;
                break;
            case 3:
                if (num_of_piaz < 10)
                    isOk = true;
                break;
            case 4:
                if (num_of_ketchap < 10)
                    isOk = true;
                break;
            case 5:
                if (num_of_mustard < 10)
                    isOk = true;
                break;
        }
        // producer producing 5 ingredients
        if (isOk) {
            for (int i = 0; i < 10; i++) {
                Ingredient item = new Ingredient(ingredientType);
                this.ingredients.add(item);
                //System.out.println("dastyare ashpaz ye : " + item.name + " sakht");
            }

            switch (ingredientType) {
                case 1:
                    num_of_goosht = Math.min(10, num_of_goosht + 5);
                    break;
                case 2:
                    num_of_goje = Math.min(10, num_of_goje + 5);
                    break;
                case 3:
                    num_of_piaz = Math.min(10, num_of_piaz + 5);
                    break;
                case 4:
                    num_of_ketchap = Math.min(10, num_of_ketchap + 5);
                    break;
                case 5:
                    num_of_mustard = Math.min(10, num_of_mustard + 5);
                    break;
            }
        }
        ongoing_time += 1;
        //System.out.println(consumer.type);
        semCon.release();


    }
}

// Producer class
class Producer implements Runnable {
    Q q;
    Consumer consumer1, consumer2;
    Producer(Q q, Consumer consumer1, Consumer consumer2) throws InterruptedException {
        this.q = q;
        this.consumer1 = consumer1;
        this.consumer2 = consumer2;
        sleep(1000);
        new Thread(this, "Producer").start();
    }

    public void run()
    {
        while (consumer1.num_of_clients != 0 || consumer2.num_of_clients != 0) {
            if (q.ongoing_time == 0 || q.ongoing_time % 20 == 0) {
                if (consumer2.num_of_clients > consumer1.num_of_clients) {
                    while (true) {
                        int random_int = (int) (Math.random() * 5 + 1);
                        if ((random_int == 1 && q.num_of_goosht < 2)
                                || (random_int == 3 && q.num_of_piaz < 3)
                                || (random_int == 4 && q.num_of_ketchap < 2)
                                || (random_int == 5 && q.num_of_mustard < 2)
                                || (random_int != 2)) {
                            q.makeIngredient(random_int);
                            try {
                                sleep(1000);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            if (q.ongoing_time % 20 == 0 || (consumer1.num_of_clients == 0 && consumer2.num_of_clients == 0))
                                break;
                            q.makeIngredient(random_int);
                            try {
                                sleep(1000);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            if (q.ongoing_time % 20 == 0 || (consumer1.num_of_clients == 0 && consumer2.num_of_clients == 0))
                                break;
                        }

                    }

                } else {
                    while (true) {
                        int random_int = (int) (Math.random() * 5 + 1);
                        if ((random_int == 1 && q.num_of_goosht < 1)
                                || (random_int == 2 && q.num_of_goje < 2)
                                || (random_int == 3 && q.num_of_piaz < 1)
                                || (random_int == 4 && q.num_of_ketchap < 2)
                                || (random_int != 5)) {
                            q.makeIngredient(random_int);
                            try {
                                sleep(50);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            if (q.ongoing_time % 20 == 0 || (consumer1.num_of_clients == 0 && consumer2.num_of_clients == 0))
                                break;
                            q.makeIngredient(random_int);
                            try {
                                sleep(50);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            if (q.ongoing_time % 20 == 0 || (consumer1.num_of_clients == 0 && consumer2.num_of_clients == 0))
                                break;
                        }
                    }
                }
            }
        }

        System.out.println("Goosht " + q.num_of_goosht + "- Goje " + q.num_of_goje+ "- Piaz " +
                q.num_of_piaz + "- Ketchup " + q.num_of_ketchap + "- Mustard " + q.num_of_mustard);

    }
}

// Consumer class
class Consumer implements Runnable {
    Q q;
    int type;
    int num_of_clients;
    Consumer(Q q, int type, int clients)
    {
        this.q = q;
        this.type = type;
        this.num_of_clients = clients;
        new Thread(this, "Consumer" + type).start();
    }

    public void run()
    {
        while (num_of_clients > 0)
            q.useIngredient(this);

    }
}

public class KarimSagPaz {
    public static void main(String[] args) throws InterruptedException {
        // creating buffer queue
        Q q = new Q();

        int con1_client_num = 0, con2_client_num = 0;
        // starting consumer thread

        Scanner sc = new Scanner(System.in);
        int n = sc.nextInt();
        for (int i = 0; i < n; i++){
            int type = sc.nextInt();
            if (type == 1)
                con1_client_num++;
            else
                con2_client_num++;
            q.orders.add(new Order(type, false, i+1));
        }

        // starting producer thread
        Consumer gordon = new Consumer(q , 1, con1_client_num);
        Consumer jamie = new Consumer(q , 2, con2_client_num);
        new Producer(q, gordon, jamie);
    }
}
