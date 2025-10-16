from fastapi import HTTPException, APIRouter, Query
from dotenv import load_dotenv
import os
import psycopg2
import pytz

router = APIRouter()


def connect_to_database():
    load_dotenv()
    db_host = os.getenv('DATABASE_HOST')
    db_port = os.getenv('DATABASE_PORT')
    db_name = os.getenv('DATABASE_NAME')
    db_user = os.getenv('DATABASE_USER')
    db_password = os.getenv('DATABASE_PASSWORD')

    conn = psycopg2.connect(
        host=db_host,
        port=db_port,
        database=db_name,
        user=db_user,
        password=db_password
    )
    return conn


def format_time(time):
    while True:
        index = time.find("+")
        if time[index - 1] == "0":
            time = time[:index - 1] + time[index:]
        else:
            break
    return time

@router.get('/v2/posts/{postid}/users')
def get_users_for_post(postid: int):
    try:
        conn = connect_to_database()
        cursor = conn.cursor()
        cursor.execute("""
            SELECT users.*
            FROM users
            JOIN comments ON comments.userid = users.id
            JOIN posts ON posts.id = comments.postid
            WHERE posts.id = %s
            ORDER BY comments.creationdate
        """, (postid,))
        users = cursor.fetchall()
        cursor.close()
        conn.close()
        formatted_users = []
        for user in users:
            formatted_user = {}
            for idx, column in enumerate(cursor.description):
                if column.name == "creationdate":
                    utc_time = user[idx].astimezone(pytz.utc)
                    formatted_user[column.name] = utc_time.strftime("%Y-%m-%dT%H:%M:%S.%f+00:00")
                    formatted_user[column.name] = format_time(formatted_user[column.name])
                elif column.name == "lastaccessdate":
                    utc_time = user[idx].astimezone(pytz.utc)
                    formatted_user[column.name] = utc_time.strftime("%Y-%m-%dT%H:%M:%S.%f+00:00")
                    formatted_user[column.name] = format_time(formatted_user[column.name])
                else:
                    formatted_user[column.name] = user[idx]
            formatted_users.append(formatted_user)

        # Construct the final response
        response = {"items": formatted_users}
        return response
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@router.get('/v2/users/{user_id}/friends')
def get_user_friends(user_id: int):
    try:
        conn = connect_to_database()
        cursor = conn.cursor()
        cursor.execute("""
            SELECT DISTINCT users.*
            FROM users
            JOIN comments ON comments.userid = users.id
            JOIN (
                    SELECT posts.id
                    FROM posts
                    WHERE posts.owneruserid = %s
                    UNION
                    SELECT comments.postid
                    FROM comments
                    WHERE comments.userid = %s
            ) AS relevant_posts ON relevant_posts.id = comments.postid
            ORDER BY users.creationdate;
        """, (user_id, user_id,))
        friends = cursor.fetchall()
        cursor.close()
        conn.close()

        formatted_friends = []
        for friend in friends:
            formatted_friend = {}
            for idx, column in enumerate(cursor.description):
                if column.name == "creationdate":
                    utc_time = friend[idx].astimezone(pytz.utc)
                    formatted_friend[column.name] = utc_time.strftime("%Y-%m-%dT%H:%M:%S.%f+00:00")
                    formatted_friend[column.name] = format_time(formatted_friend[column.name])
                elif column.name == "lastaccessdate":
                    utc_time = friend[idx].astimezone(pytz.utc)
                    formatted_friend[column.name] = utc_time.strftime("%Y-%m-%dT%H:%M:%S.%f+00:00")
                    formatted_friend[column.name] = format_time(formatted_friend[column.name])
                else:
                    formatted_friend[column.name] = friend[idx]
            formatted_friends.append(formatted_friend)

        response = {"items": formatted_friends}
        return response
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@router.get('/v2/tags/{tagname}/stats')
def get_tag_stats(tagname: str):
    try:
        conn = connect_to_database()
        cursor = conn.cursor()

        cursor.execute("""
            WITH TotalPosts AS (
                SELECT DISTINCT
                    EXTRACT(DOW FROM p.creationdate) AS day_of_week,
                    COUNT(DISTINCT p.id) AS total_posts
                FROM
                    posts p
                GROUP BY
                    EXTRACT(DOW FROM p.creationdate)
            ),
            TagPosts AS (
                SELECT DISTINCT
                    EXTRACT(DOW FROM p.creationdate) AS day_of_week,
                    COUNT(*) AS tag_posts
                FROM
                    posts p
                JOIN
                    post_tags pt ON p.id = pt.post_id
                JOIN
                    tags t ON pt.tag_id = t.id
                WHERE
                    t.tagname = %s
                GROUP BY
                    EXTRACT(DOW FROM p.creationdate)
            )
            SELECT
                t.day_of_week,
                COALESCE(ROUND(CAST(tp.tag_posts AS NUMERIC) / t.total_posts * 100, 2), 0) AS tag_percentage
            FROM
                TotalPosts t
            LEFT JOIN
                TagPosts tp ON t.day_of_week = tp.day_of_week
            ORDER BY
                t.day_of_week;
        """, (tagname,))

        tag_stats = cursor.fetchall()
        cursor.close()
        conn.close()
        response = {
            "result": {
                "monday": 0,
                "tuesday": 0,
                "wednesday": 0,
                "thursday": 0,
                "friday": 0,
                "saturday": 0,
                "sunday": 0
            }
        }
        i = 1
        for day in response["result"]:
            if i == 7:
                i = 0
            response["result"][day] = tag_stats[i][1]
            i += 1

        return response
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@router.get('/v2/posts/')
def get_recent_resolved_posts(duration: int = Query(None), limit: int = Query(...), query: str = Query(None)):
    try:
        conn = connect_to_database()
        cursor = conn.cursor()
        if duration:
            cursor.execute("""
                SELECT
                    id,
                    creationdate,
                    viewcount,
                    lasteditdate,
                    lastactivitydate,
                    title,
                    closeddate,
                    ROUND(EXTRACT(EPOCH FROM (closeddate - creationdate)) / 60, 2) AS duration
                FROM
                    posts
                WHERE
                    posttypeid = 1  
                    AND closeddate IS NOT NULL
                    AND EXTRACT(EPOCH FROM (closeddate - creationdate)) / 60 <= %s
                ORDER BY
                    creationdate DESC
                LIMIT %s;
            """, (duration, limit))

            recent_posts = cursor.fetchall()
            cursor.close()
            conn.close()
            formatted_posts = []
            for post in recent_posts:
                formatted_post = {
                    "closeddate": format_time(post[6].astimezone(pytz.utc).strftime("%Y-%m-%dT%H:%M:%S.%f+00:00")),
                    "creationdate": format_time(post[1].astimezone(pytz.utc).strftime("%Y-%m-%dT%H:%M:%S.%f+00:00")),
                    "duration": post[7],
                    "id": post[0],
                    "lastactivitydate": format_time(post[4].astimezone(pytz.utc).strftime("%Y-%m-%dT%H:%M:%S.%f+00:00")),
                    "lasteditdate": post[3],
                    "title": post[5],
                    "viewcount": post[2]
                }
                formatted_posts.append(formatted_post)
            response = {"items": formatted_posts}
            return response
        elif query:
            cursor.execute("""
                SELECT
                    p.id,
                    p.creationdate,
                    p.viewcount,
                    p.lasteditdate,
                    p.lastactivitydate,
                    p.title,
                    p.body,
                    p.answercount,
                    p.closeddate,
                    ARRAY_AGG(t.tagname) AS tags
                FROM
                    posts p
                JOIN
                    post_tags pt ON p.id = pt.post_id
                JOIN
                    tags t ON pt.tag_id = t.id
                WHERE
                    LOWER(p.title) LIKE LOWER(%s)
                    OR LOWER(p.body) LIKE LOWER(%s)
                GROUP BY 
                    p.id, 
                    p.creationdate, 
                    p.viewcount, 
                    p.lasteditdate, 
                    p.lastactivitydate, 
                    p.title, 
                    p.body, 
                    p.answercount, 
                    p.closeddate
                ORDER BY
                    p.creationdate DESC
                LIMIT %s;
            """, ('%' + query.lower() + '%', '%' + query.lower() + '%', limit))
            posts = cursor.fetchall()
            cursor.close()
            conn.close()
            formatted_posts = []
            for post in posts:
                formatted_post = {
                    "answercount": post[7],
                    "body": post[6],
                    "closeddate": post[8],
                    "creationdate": format_time(post[1].astimezone(pytz.utc).strftime("%Y-%m-%dT%H:%M:%S.%f+00:00")),
                    "id": post[0],
                    "lastactivitydate": format_time(post[4].astimezone(pytz.utc).strftime("%Y-%m-%dT%H:%M:%S.%f+00:00")),
                    "lasteditdate": post[3],
                    "tags": post[9],
                    "title": post[5],
                    "viewcount": post[2]
                }
                formatted_posts.append(formatted_post)
            response = {"items": formatted_posts}
            return response
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

